/*
 *  Copyright (c) 2011 Jakub Szafrański <samu@pirc.pl>
 * 
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 */
 
 /*
  *  FreeBSD MOTD
  *  
  *  Compile this with:
  *  gcc -lkvm -lc motd.c -o motd
  * 
  */
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <kvm.h>
#include <ufs/ufs/quota.h>

#define _POSIX2_LINE_MAX 2048
#define O_RDONLY 0x0000
#define _PATH_DEVNULL "/dev/null"

int main() {
    register struct passwd *pw;
    register uid_t uid;	
    int i, proc_count;
    char *cuname;
    char buf[_POSIX2_LINE_MAX];
    const char *execf, *coref;
    const char* path = "/home"; 
    time_t now;
    static kvm_t *kd;
    struct kinfo_proc *kp;
    struct rlimit rlp;
    struct dqblk *my_st;
    static int nproc;
    static struct kinfo_proc *plist;
    
    time(&now);
    uid = geteuid ();
    pw = getpwuid (uid);

    cuname = "-ERROR-";
    if(pw) cuname = pw->pw_name;

    execf = NULL;
    coref = _PATH_DEVNULL;
    proc_count = 0;
   
    kd = kvm_openfiles(execf, coref, NULL, O_RDONLY, buf);
    plist = kvm_getprocs(kd, KERN_PROC_PROC, 0, &nproc);
   
    for (i = 0, kp = plist; i < nproc; i++, kp++) if(kp->ki_uid == uid) proc_count++;

    getrlimit(RLIMIT_NPROC, &rlp);

    
    my_st = malloc(sizeof(struct dqblk));
    quotactl(path,QCMD(Q_GETQUOTA,USRQUOTA),uid,my_st);

    /*
     *   ctime(&now);           data
     *   cuname                 kanoniczna nazwa uzytkownika
     *   proc_count             liczba procesow uzytkownika
     *   rpl.rlim_max           limit procesow uzytkownika
     *   my_st->dqb_curblocks   liczba blokow uzywanych przez usera dla mountpointu path (NOTA: dla UFS 1 block = 512B)
     *   my_st->dqb_bhardlimit  limit blokow (quota)
     */
     
    printf("\n");
    printf("                \033[1;31m ,       \033[0;0m \033[1;31m,\033[0;0m \033[1;31m       Witaj na serwerze samu.pl\033[0;0m\n");
    printf("                \033[1;31m/(      \033[0;0m  \033[1;31m)`\033[0;0m\n");
    printf("                \033[1;31m\\ \\___\033[0;0m   \033[1;31m/ |\033[0;0m       \033[2;31m=\033[0;0m\033[0;37m[\033[0;0m \033[1;1m   Data:\033[0;0m \033[0;37m%s\033[0;0m", ctime(&now)); // bez \n, bo data dodaje, kij wie czemu
    printf("                \033[1;31m/ \033[0;0m _\033[1;31m  `-/  '\033[0;0m \n");
    printf("               \033[1;31m(\033[0;0m/\\/ \\\033[1;31m \\   /\033[0;0m        \033[2;31m=\033[0;0m\033[0;37m[\033[0;0m \033[1;1m  Login:\033[0;0m \033[0;37m%s\033[0;0m\n", cuname);
    printf("               / /  |  \033[1;31m`    \033[0;0m       \033[2;31m=\033[0;0m\033[0;37m[\033[0;0m \033[1;1mProcesy:\033[0;0m\033[0;37m %d / %d \033[0;0m\n", proc_count, rlp.rlim_max);
    printf("               \033[1;2mO\033[0;0m \033[0;0m\033[1;2mO\033[0;0m  )\033[1;31m /     |\033[0;0m      \033[2;31m=\033[0;0m\033[0;37m[\033[0;0m \033[1;1m  Quota:\033[0;0m \033[0;37m%dM / %dM \033[0;0m\n", my_st->dqb_curblocks/2048, my_st->dqb_bhardlimit/2048);
    printf("               `-^--'\033[1;31m`<     '\033[0;0m \n");
    printf("              \033[1;31m(_.)  _  )   /\033[0;0m       \033[2;31m=\033[0;0m\033[0;37m[\033[0;0m \033[0;37mListe vhostow otrzymasz wpisujac\033[0;0m\n");
    printf("               \033[1;31m`.___/`    /\033[0;0m        \033[2;31m=\033[0;0m\033[0;37m[\033[0;0m \033[0;37mpolecenie\033[0;0m vhost\n");
    printf("                 \033[1;31m`-----' /\033[0;0m \n");
    printf("    \033[1;31m<----.    \033[0;0m \033[1;31m__ / __ \033[0;0m \n");
    printf("    \033[1;31m<----|====\033[0;0m\033[1;31mO)))\033[0;0m\033[1;31m==\033[0;0m\033[1;31m) \\) /\033[0;0m\033[1;31m==== \033[0;0m \n");
    printf("    \033[1;31m<----    \033[0;0m \033[1;31m`--' `.__,' \033[0;0m\n");
    printf("                 \033[1;31m|        |\033[0;0m \n");
    printf("                 \033[1;31m \\       /       /\\ \033[0;0m \n");
    printf("             \033[1;2m______\033[0;0m\033[1;31m( (\033[0;0m\033[1;2m_\033[0;0m  \033[1;31m/ \\______/\033[0;0m \n");
    printf("           \033[1;2m,'  ,-----'   |\033[0;0m \n");
    printf("           \033[1;2m`--{__________)\033[0;0m \n\n\n");

    free(my_st);
    return 0;
}
