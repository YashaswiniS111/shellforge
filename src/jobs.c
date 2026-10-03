#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "jobs.h"

#define MAX_JOBS 100

static job_t job_table[MAX_JOBS];
static int next_job_id = 1;

void jobs_init(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        job_table[i].job_id = 0;
        job_table[i].pgid = 0;
        job_table[i].state = JOB_DONE;
        job_table[i].command[0] = '\0';
    }

    next_job_id = 1;
}

int job_add(pid_t pgid, const char *command, job_state_t state)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].job_id == 0)
        {
            job_table[i].job_id = next_job_id++;
            job_table[i].pgid = pgid;
            job_table[i].state = state;

            if (command != NULL)
            {
                strncpy(job_table[i].command,
                        command,
                        sizeof(job_table[i].command) - 1);

                job_table[i].command[
                    sizeof(job_table[i].command) - 1
                ] = '\0';
            }
            else
            {
                job_table[i].command[0] = '\0';
            }

            return job_table[i].job_id;
        }
    }

    fprintf(stderr, "shellforge: maximum number of jobs reached\n");
    return -1;
}

job_t *job_find(int job_id)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].job_id == job_id)
        {
            return &job_table[i];
        }
    }

    return NULL;
}

job_t *job_find_by_pgid(pid_t pgid)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].job_id != 0 &&
            job_table[i].pgid == pgid)
        {
            return &job_table[i];
        }
    }

    return NULL;
}

void job_remove(int job_id)
{
    job_t *job = job_find(job_id);

    if (job == NULL)
        return;

    job->job_id = 0;
    job->pgid = 0;
    job->state = JOB_DONE;
    job->command[0] = '\0';
}

void jobs_print(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (job_table[i].job_id == 0)
            continue;

        const char *state;

        switch (job_table[i].state)
        {
            case JOB_RUNNING:
                state = "Running";
                break;

            case JOB_STOPPED:
                state = "Stopped";
                break;

            case JOB_DONE:
                state = "Done";
                break;

            default:
                state = "Unknown";
                break;
        }

        printf("[%d] %-8s %s\n",
               job_table[i].job_id,
               state,
               job_table[i].command);
    }
}

void job_stop(pid_t pgid)
{
    job_t *job = job_find_by_pgid(pgid);

    if (job == NULL)
        return;

    job->state = JOB_STOPPED;
}

void job_continue(pid_t pgid)
{
    job_t *job = job_find_by_pgid(pgid);

    if (job == NULL)
        return;

    if (kill(-pgid, SIGCONT) < 0)
    {
        perror("kill");
        return;
    }

    job->state = JOB_RUNNING;
}

void job_done(pid_t pgid)
{
    job_t *job = job_find_by_pgid(pgid);

    if (job == NULL)
        return;

    job->state = JOB_DONE;
}
