#include "evaluation.h"
#include "utility.h"
#include "profiler.h"
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<fcntl.h>
#include<errno.h>
#define PIPE_IN 0 
#define PIPE_OUT 1

void evaluate(double* time_usr, double* time_sys, double* memory, double* failNum){
	gint fd[2], nbytes;
	pid_t   childpid;
	gchar readbuffer[128];
	g_snprintf(readbuffer, 128, "%lf", timeout_sec);
	pipe(fd);

	if((childpid = fork()) == -1)
	{
		perror("fork");
		exit(1);
	}

	if(childpid == 0)
	{
		/* Child process closes up input side of pipe */
		close(fd[0]);

		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		execl("memory", "memory", CURRDIR, TESTCASEDIR, profile_times==0?"1":"0", readbuffer, (gchar*)0);
		perror("exec");
		exit(0);
	}
	else{
		/* Parent process closes up output side of pipe */
		close(fd[1]);

		/* Read in a string from the pipe */
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
		readbuffer[nbytes]='\0';
		close(fd[0]);
		//g_printf("read:%s", readbuffer);
		sscanf(readbuffer, "%lf %lf %lf %lf", time_usr, time_sys, memory, failNum);
	}
}

double evaluateIndividual(individual* program, gint index){
	program->time=-1;
	program->memory=-1;
	program->failNum=-1;
	gchar* filename=g_malloc0(32*sizeof(gchar));
	g_snprintf(filename, 32, "libmalloc.so");
	if(!saveIndividual(program, filename, 1)){
		g_free(filename);
		g_printf("save individual in evaluateIndividual() fail.\n");
		return 0;
	}
	double time_usr, time_sys, memory, failNum;
	evaluate(&(time_usr), &(time_sys), &(program->memory), &(program->failNum));

	//program->time_usr=program->evaluateTimes/(double)(program->evaluateTimes+1)*program->time_usr+1/(double)(program->evaluateTimes+1)*time_usr;
	//program->time_sys=program->evaluateTimes/(double)(program->evaluateTimes+1)*program->time_sys+1/(double)(program->evaluateTimes+1)*time_sys;
	program->time_usr=time_usr;
	program->time_sys=time_sys;
	program->time=program->time_usr+program->time_sys;
	program->evaluateTimes++;
	program->time_repeat[0]=program->time;
	gint i;
	if(program->failNum>0){
		program->time_usr=1e10;
		program->time_sys=1e10;
		program->time=1e10;
		program->memory=1e10;
		g_printf("#");
		fprintf(logfp, "#");
		fflush(stdout);
	}
	else{
		g_printf(".");
		fprintf(logfp, ".");
		fflush(stdout);
/**/
		program->time=0;
		saveIndividual(program, filename, 0);
		for(i=0; i<REPEAT; i++){
			evaluate(&time_usr, &time_sys, &memory, &failNum);
			program->time_repeat[i]=time_usr+time_sys;
			program->time+=program->time_repeat[i];
			if(program->time_repeat[i]>ori->time+1){
				break;
			}
		}
		if(i<REPEAT){
			i++;
		}
		program->time/=i;

	}
	profile_times++;
	g_free(filename);
	return 0;
}
