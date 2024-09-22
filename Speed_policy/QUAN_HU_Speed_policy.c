#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define true 1
#define false 0

typedef struct _job {
    int index; //Index of the job (which also indicates the priority)
    int O; //Release time
    int C; //Computation time
    int D; //Absolute Deadline
} Job;

typedef struct _jobSet {
    Job * job; //head of the list
    struct _jobSet * next;
} JobSet;

typedef struct _interval {
    int ts;
    int tf;
} Interval;

typedef struct _set {
    Job * job; 
    int TE;
    int TL;
    float speed;
    Interval essential_interval;
    struct _set * next; 
} Set;

typedef struct _intervalSpeed {
    Interval interval;
    float S;
} IntervalSpeed;

typedef struct _intervalSet {
    Interval interval;
    struct _intervalSet * next;
} IntervalSet;

typedef struct _speedSet {
    float S;
    struct _speedSet * next;
} SpeedSet;

typedef struct _intervalsSpeeds {
    IntervalSet * intervalSet;
    SpeedSet * speedSet;
} IntervalsSpeeds;

typedef struct _schedulingPoints {
    int val;
    int index;
    Job * job;
    struct _schedulingPoints * next;
} SchedulingPoints;


IntervalSet * create_interval_set (IntervalSet * is, Interval interval) {
    IntervalSet * new = (IntervalSet *) malloc (sizeof(IntervalSet));
    new->interval = interval;
    new->next = is;
    return new;
}

SpeedSet * create_speed_set (SpeedSet * ss, float speed) {
    SpeedSet * new = (SpeedSet *) malloc (sizeof(SpeedSet));
    new->S = speed;
    new->next = ss;
    return new;
}

JobSet * create_job_set (JobSet * js, Job * job) {
    JobSet * new = (JobSet *) malloc (sizeof(JobSet));
    new->job = job;
    new->next = js;
    return new;
}

Set * create_set (Set * s, Job * job, int te, int tl, float speed, Interval interval) {
    Set * new = (Set *) malloc (sizeof(Set));
    new->job = job;
    new->TE = te;
    new->TL = tl;
    new->speed = speed;
    new->essential_interval = interval;
    new->next = s;
    return new;
}

SchedulingPoints * create_scheduling_point (SchedulingPoints * sp, int val, Job * job) {
    SchedulingPoints * new = (SchedulingPoints *) malloc (sizeof(SchedulingPoints));
    new->val = val;
    new->job = job;


    //We sort at the beginning
    if (sp == NULL) {
        new->index = 0;
        new->next = sp;
        return new;
    }

    else {
        SchedulingPoints * curr = sp; 
        if (new->val <= sp->val) {
            new->next = sp;
            new->index = 0;
            sp = new;
            curr = sp;
        }
        else {
            int i = 1;
            while (curr->next != NULL) {
                if (new->val <= curr->next->val) {
                    new->next = curr->next;
                    curr->next = new;
                    new->index = i;
                    break;
                }
                i++;
                curr = curr->next;
            }
            if (curr->next == NULL) {
                curr->next = new;
                new->next = NULL;
                new->index = i;
            }
        }
        curr = curr->next->next; //We go to the element next to the one we just added
        while (curr) {
            curr->index++;
            curr = curr->next;
        }
        return sp;
    }
    
}

/* Computes the earliest scheduling point of a job Jn based on a set of jobs */
int earliest_scheduling_point (Job j, JobSet * js) {
    
    JobSet * current = js; //pointer on the current job
    JobSet * tmp = NULL; 
    int TE = j.O; //We initialise the earliest scheduling point to the current job release

    while (current != NULL){
       
        if (current->job->O >= 0 && current->job->O <= j.O) { //First, we check if the considered scheduling point is in [0, Rn]
            
            int found = true;
            
            tmp = js;
            while (tmp != NULL) {
                if (current->job->O > tmp->job->O && current->job->O < tmp->job->D) {
                    found = false;
                    break;
                }
                tmp = tmp->next;
            }
            
            if (found) {
                if (current->job->O < TE) TE = current->job->O;
            }
        }
        current = current->next;
    }
    return TE;
}

/* Returns the latest scheduling point of a job Jn */
int latest_scheduling_point (Job j) {  
    return j.D;
}

float minimum_constant_speed (JobSet * js, int ta, int tb) {
    
    JobSet * current = js; //pointer on the current job
    int sum = 0; //sum of the computation times in [ta, tb]

    while (current != NULL) {
        if (current->job->O >= ta && current->job->O < tb) { //We check if the current job is computed in [ta, tb]
            sum += current->job->C;
        }
        current = current->next;
    }
    
    return ((float) sum)/(tb-ta);
}

int ** init_matrix (int n) {

    int ** mat = (int **) malloc (sizeof (int *) * n);
    for (int i = 0; i < n; i++) {
        mat[i] = (int *) malloc (sizeof (int) * n);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i <= j) { //Case where tb <= ta
                mat[i][j] = 0;
            }
            else {
                mat[i][j] = -1;
            }
        }
    }

    return mat;
}



IntervalSpeed essential_interval_Jn (int TE, int TL, JobSet * js, Job job) {

    SchedulingPoints ta_prim = {job.O, -1, &job, NULL};
    SchedulingPoints tb = {job.O, -1, &job, NULL};
    SchedulingPoints ta = {TE, -1, &job, NULL};
    SchedulingPoints tb_prim = {TL, -1, &job, NULL};

    //We create a set of j-scheduling points
    int nb_jobs = 0;
    JobSet * curr_job = js;
    SchedulingPoints * sp = NULL;
    while (curr_job != NULL) {
        sp = create_scheduling_point(sp, curr_job->job->O, curr_job->job);
        curr_job = curr_job->next;
        nb_jobs++;
    }
    sp = create_scheduling_point(sp, job.D, &job);


    //We create our matrix of the sums of workloads according to the different scheduling points
    int ** mat = init_matrix(nb_jobs+1);
    SchedulingPoints * tmp = sp;
    while (tmp->next) {
        mat[tmp->index+1][tmp->index] = tmp->job->C;
        tmp = tmp->next;
    }

    for (int k = 2; k <= nb_jobs; k++) {
        int i = k;
        int j = 0;
        while (i <= nb_jobs && j <= nb_jobs) {
            mat[i][j] = mat[i-1][j] + mat[i][j+1] - mat[i-1][j+1];
            i++;
            j++;
        }
    }


    /* for (int i = 0; i <= nb_jobs; i++){
        for (int j = 0; j <= nb_jobs; j++) {
            printf("mat[%d][%d] : %d   ", i, j, mat[i][j]);
        }
        printf("\n");
    } */


    tmp = sp;
    while (tmp != NULL) {
        if (tmp->val == ta_prim.val) {
            ta_prim.index = tmp->index;
            break;
        }
        tmp = tmp->next;
    }

    
    
    while (ta.val != ta_prim.val || tb_prim.val != tb.val) {
        ta.val = ta_prim.val;
        ta.index = ta_prim.index;
        tb_prim.val = tb.val;
        tb_prim.index = tb.index;

        float min_speed = -1;
        SchedulingPoints * curr_sp = sp;
        while (curr_sp != NULL) {
            if (curr_sp->val >= ta.val && curr_sp->val <= TL && curr_sp->val - ta.val != 0) {
                if (min_speed == -1){
                    min_speed = ((float) mat[curr_sp->index][ta.index]) / (curr_sp->val - ta.val);
                    tb.val = curr_sp->val;
                    tb.index = curr_sp->index;
                }
                else {
                    float speed = ((float) mat[curr_sp->index][ta.index]) / (curr_sp->val - ta.val);
                    if (speed < min_speed) {
                        min_speed = speed;
                        tb.val = curr_sp->val;
                        tb.index = curr_sp->index;
                    }
                }
            }
            curr_sp = curr_sp->next;
        }

        float max_speed = -1;
        curr_sp = sp;
        while (curr_sp != NULL) {
            if (curr_sp->val >= TE && curr_sp->val <= ta.val && tb.val - curr_sp->val != 0) {
                float speed = ((float) mat[tb.index][curr_sp->index]) / (tb.val - curr_sp->val);
                if (speed > max_speed) {
                    max_speed = speed;
                    ta_prim.val = curr_sp->val;
                    ta_prim.index = curr_sp->index;
                }
            }
            curr_sp = curr_sp->next;
        }

    }

    IntervalSpeed res;
    res.interval.ts = ta.val;
    res.interval.tf = tb.val;
    res.S = minimum_constant_speed(js, ta.val, tb.val);
    
    return res;
}

void print_interval (Interval i) {
    printf("Interval - ts : %d, tf : %d", i.ts, i.tf);
}

void print_intervals (IntervalSet * arg) {
    IntervalSet * i = arg;
    while (i) {
        print_interval(i->interval);
        printf("\n");
        i = i->next;
    }
}

void print_speeds (SpeedSet * arg) {
    SpeedSet * s = arg;
    while (s) {
        printf("Speed : %.2f\n", s->S);
        s = s->next;
    }
}

void print_job (Job j) {
    printf("Job - index : %d, O : %d, C : %d, D : %d", j.index, j.O, j.C, j.D);
}

void print_job_set (JobSet * arg) {
    JobSet * js = arg;
    while (js) {
        print_job(*js->job);
        if (js->next == NULL) {
            printf(" - Last job\n");
        }
        else {
            printf(" - Next job index : %d\n", js->next->job->index);
        }
        js = js->next;
    }
}

void print_set (Set * arg) {
    Set * s = arg;
    while (s) {
        print_job(*s->job);
        printf(", ");
        print_interval(s->essential_interval);
        printf(", Speed : %.2f, TE : %d, TL : %d\n", s->speed, s->TE, s->TL);
        s = s->next;
    }
}

IntervalsSpeeds critical_intervals (JobSet * js) {

    JobSet * curr_job = js; //pointer on the current job
    Set * set = NULL; //combination of A and S_prim

    while (curr_job != NULL) {
        int TE, TL;
        TE = earliest_scheduling_point(*curr_job->job, js);
        TL = latest_scheduling_point(*curr_job->job);

        IntervalSpeed interval_speed = essential_interval_Jn(TE, TL, js, *curr_job->job);

        set = create_set(set, curr_job->job, TE, TL, interval_speed.S, interval_speed.interval);

        curr_job = curr_job->next;
    }

    IntervalSet * T = NULL;
    SpeedSet * S = NULL;

    while (set != NULL) {
        
        //We select S_j, i.e. the maximum constant speed, and the corresponding interval 
        float S_j = set->speed;
        Interval A_j = set->essential_interval;
        int j = set->job->index;

        /*while (curr_set->next != NULL) {
            if (S_j > curr_set->next->speed) {
                S_j = curr_set->next->speed;
                A_j = curr_set->next->essential_interval;
                j = curr_set->next->job->index;
                tmp = curr_set->next;
                prec = curr_set;
            }
            curr_set = curr_set->next;
        }*/

        //We add S_j and [ts, tf] respectively to S and T
        T = create_interval_set(T, A_j);
        S = create_speed_set(S, S_j);

        //We remove S_j and [ts, tf] respectively from S_prim and A
        /* if (prec == NULL) {
            set = set->next;
            free(tmp);
        }
        else {
            prec->next = tmp->next;
            free(tmp);
        } */
        Set * tmp = set;
        set = set->next;
        free(tmp);

        int m;
        Set * curr_set = set;
        Set * prec = NULL;
        while (curr_set != NULL) {
            m = curr_set->job->index;
            if (m <= j && curr_set->job->O >= A_j.ts && curr_set->job->O < A_j.tf) {
                
                //Remove Jm from J, the set of jobs & Remove Sm from S and corresponding [ts, tf] from A
                if (prec == NULL) {
                    set = set->next;
                    free(curr_set);
                    curr_set = set;
                }
                else {
                    prec->next = curr_set->next;
                    free(curr_set);
                    curr_set = prec->next;
                }
            }
            else {
                if (curr_set->job->O > A_j.ts && curr_set->job->O <= A_j.tf) {
                    curr_set->job->O = A_j.ts;
                }
                if (curr_set->TE > A_j.ts && curr_set->TE <= A_j.tf) {
                    curr_set->TE = A_j.ts;
                }
                if (curr_set->TL > A_j.ts && curr_set->TL <= A_j.tf) {
                    curr_set->TL = A_j.ts;
                }

                prec = curr_set;
                curr_set = curr_set->next;
            }
        }


        curr_set = set;
        while (curr_set != NULL) {
            if (curr_set->job->O > A_j.tf) {
                curr_set->job->O = curr_set->job->O - (A_j.tf - A_j.ts);
            }
            if (curr_set->job->D > A_j.tf) {
                curr_set->job->D = curr_set->job->D - (A_j.tf - A_j.ts);
            }
            curr_set = curr_set->next;
        } 

        curr_set = set;
        while (curr_set != NULL) {
            IntervalSpeed interval_speed = essential_interval_Jn (curr_set->TE, curr_set->TL, js, *curr_set->job);
            curr_set->essential_interval = interval_speed.interval;
            curr_set->speed = interval_speed.S;
            curr_set = curr_set->next;
        }
    } 

    IntervalsSpeeds res;
    res.intervalSet = T;
    res.speedSet = S;

    return res;

}

void print_intervals_speeds (IntervalsSpeeds arg) {
    SpeedSet * s = arg.speedSet;
    IntervalSet * i = arg.intervalSet;

    while (s && i) {
        
        printf("Speed : %.2f, Interval : ts - %d, tf - %d\n", s->S, i->interval.ts, i->interval.tf);

        s = s->next;
        i = i->next;
    }
}

int main (int argc, char * argv[]) {
    
    //We create 3 jobs
    Job j3 = {3, 3, 5, 10};
    Job j2 = {2, 2, 4, 8};
    Job j1 = {1, 0, 1, 9};

    JobSet * js = NULL;
    js = create_job_set(js, &j3);
    js = create_job_set(js, &j2);
    js = create_job_set(js, &j1);

    print_job_set(js);
    printf("\n");

    IntervalsSpeeds res = critical_intervals(js);
    
    printf("Final results: \n");
    print_intervals_speeds(res);

    return 0;
}