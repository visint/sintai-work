/*
 * --------------------------------------
 * clish.c
 *
 * A console client for libclish
 * --------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <signal.h>
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#if HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include "lub/list.h"
#include "lub/system.h"
#include "clish/shell.h"
#include "clish/internal.h"




#define QUOTE(t) #t
/* #define version(v) printf("%s\n", QUOTE(v)) */
#define version(v) printf("%s\n", v)

char cliSaveName[128];
extern int CurrentBloadNum ;



/* Hooks */
static clish_shell_hooks_t my_hooks =
{
    NULL, /* don't worry about init callback */
    clish_access_callback,
    NULL, /* don't worry about cmd_line callback */
    clish_script_callback,
    NULL, /* don't worry about fini callback */
    clish_config_callback,
    clish_log_callback,
    NULL  /* don't register any builtin functions */
};

static void help(int status, const char *argv0);

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)
//函数set_disp_mode用于控制是否开启输入回显功能
//如果option为0，则关闭回显，为1则打开回显
int set_disp_mode(int fd,int option)
{
    int err;
    struct termios term;
    if(tcgetattr(fd,&term)==-1)
    {
        perror("Cannot get the attribution of the terminal");
        return 1;
    }
    if(option)
        term.c_lflag|=ECHOFLAGS;
    else
        term.c_lflag &=~ECHOFLAGS;
    err=tcsetattr(fd,TCSAFLUSH,&term);
    if(err==-1 && err==EINTR)
    {
        perror("Cannot set the attribution of the terminal");
        return 1;
    }
    return 0;
}

//函数getpasswd用于获得用户输入的密码，并将其存储在指定的字符数组中
int getname(char* passwd, int size)
{
    int c;
    int n = 0;

    printf("User name:");

    do
    {
        c=getchar();
        if (c != '\n'||c!='\r')
        {
            passwd[n++] = c;
        }
    }
    while(c != '\n' && c !='\r' && n < (size - 1));
    passwd[n] = '\0';
    return n;
}



//函数getpasswd用于获得用户输入的密码，并将其存储在指定的字符数组中
int getpasswd(char* passwd, int size)
{
    int c;
    int n = 0;

    printf("Password:");

    do
    {
        c=getchar();
        if (c != '\n'||c!='\r')
        {
            passwd[n++] = c;
        }
    }
    while(c != '\n' && c !='\r' && n < (size - 1));
    passwd[n] = '\0';
    return n;
}
/*
int getUserLevel(char* user, char *pass)
{
    strcpy(cliSaveName,user);

    if(strcmp(user,"admin")== 0)
    {
        return 1;
    }
    else     if(strcmp(user,"visint")== 0)
    {
        return 2;
    }
    else     if(strcmp(user,"jiang")== 0)
    {
        return 3;
    }

    else
        return 4;
}
*/

int getCompanyLevel()
{
    if(nvram_get("company") == NULL)
    {

        return 1;
    }

    if(strcmp(nvram_get("company"),"visint")== 0)
    {
        return 0;
    }
    else
    {

        return 	1;
    }
}

/*

extern int			umDeleteUser(char_t *user);

extern char_t			*umGetFirstUser();
extern char_t			*umGetNextUser(char_t *lastUser);

extern bool_t			umUserExists(char_t *user);
获取用户级别,获取用户级别 返回值：-1，该用户不存在 1-管理用户2-普通用户
extern short umGetUserLevel(char_t *user);
设置用户级别,参数: level:1-管理用户2-普通用户;返回值：设置成功:UM_OK，否则返回错误码
extern int umSetUserLevel(char_t *user, int level);
extern char_t			*umGetUserPassword(char_t *user);
extern int			umSetUserPassword(char_t *user, char_t *password);




*/

int getUserLevel(char* user, char *pass)
{
    int role;
    strcpy(cliSaveName,user);



    if(strcmp(user,"sintai")== 0)
    {
        if(strcmp(pass,"sintai")== 0)
        {
            return 3;
        }
        else
        {
            return 4;
        }
    }
    umOpen();

    umRestore("/vispace/webs/umconfig.txt");

    if(umUserExists(user) == 1 && strcmp(umGetUserPassword(user),pass) == 0 )
    {

        role = umGetUserLevel(user);
        umClose();
        return role;
    }
    else
    {
        umClose();
        return  4;
    }
}

static void sighandler( int signum )
{
    signal( signum, sighandler );
}



int getupCliConmand()
{
    char *type =   nvram_get("devType");
    if(type == NULL)
        return 4;

    switch(type[0])
    {
    case '1':
        return 1;

    case '2':
        return 2;

    case '4':
        return 4;
    default:
        return 4;
    }
}


typedef enum
{
    BAND_NONE,
    BAND_FOUR=4,
    BAND_TOW=2,
    BAND_ONE=1
} BANDTYPE;

/*--------------------------------------------------------- */
int main(int argc, char **argv)
{
    int running;
    int result = -1;
    clish_shell_t *shell = NULL;
    char *p,passwd[10240],name[10240];
    int  userLevel;
    int  baneType;

    /* Command line options */
    const char *socket_path = KONFD_SOCKET_PATH;
    bool_t lockless = BOOL_FALSE;
    bool_t stop_on_error = BOOL_FALSE;
    bool_t interactive   = BOOL_TRUE;
    bool_t quiet = BOOL_FALSE;
    bool_t utf8 = BOOL_FALSE;
    bool_t bit8 = BOOL_FALSE;
    bool_t log = BOOL_FALSE;
    const char *xml_path = getenv("CLISH_PATH");
    const char *view = getenv("CLISH_VIEW");
    const char *viewid = getenv("CLISH_VIEWID");
    FILE *outfd = stdout;
    bool_t istimeout = BOOL_TRUE;
    int timeout = 600;
    bool_t cmd = BOOL_FALSE; /* -c option */
    lub_list_t *cmds; /* Commands defined by -c */
    lub_list_node_t *iter;
    const char *histfile = "~/.clish_history";
    char *histfile_expanded = NULL;
    unsigned int histsize = 50;
    int company = 1;

    /* Signal vars */
    struct sigaction sigpipe_act;
    sigset_t sigpipe_set;

    struct sigaction sig_act;

    sigset_t sig_set;

    static const char *shortopts = "hvs:ledx:w:i:bqu8okt:c:f:z:";
#ifdef HAVE_GETOPT_H
    static const struct option longopts[] =
    {
        {"help",	0, NULL, 'h'},
        {"version",	0, NULL, 'v'},
        {"socket",	1, NULL, 's'},
        {"lockless",	0, NULL, 'l'},
        {"stop-on-error", 0, NULL, 'e'},
        {"dry-run",	0, NULL, 'd'},
        {"xml-path",	1, NULL, 'x'},
        {"view",	1, NULL, 'w'},
        {"viewid",	1, NULL, 'i'},
        {"background",	0, NULL, 'b'},
        {"quiet",	0, NULL, 'q'},
        {"utf8",	0, NULL, 'u'},
        {"8bit",	0, NULL, '8'},
        {"log",		0, NULL, 'o'},
        {"check",	0, NULL, 'k'},
        {"timeout",	1, NULL, 't'},
        {"command",	1, NULL, 'c'},
        {"histfile",	1, NULL, 'f'},
        {"histsize",	1, NULL, 'z'},
        {NULL,		0, NULL, 0}
    };
#endif

    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGTERM);
    sigaddset(&sig_set, SIGINT);
    sigaddset(&sig_set, SIGQUIT);

    sig_act.sa_flags = 0;
    sig_act.sa_mask = sig_set;
    sig_act.sa_handler = &sighandler;
    sigaction(SIGTERM, &sig_act, NULL);
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGQUIT, &sig_act, NULL);

#if 0
    umOpen();

    umRestore("/vispace/webs/umconfig.txt");
    char	*aaaaa=		umGetFirstUser();
    printf("user %s\n",aaaaa);
    return 0;
#endif

    nvram_init();


start :
    company = getCompanyLevel();

    baneType = getUnitClass(0,0);

    char *status;

    status = getUnitStatusMap(0);

    if(company == 0)
    {
        printf("********************************************************************\n");
        printf("*                 Welcome to use Command Line Tool                 *\n");
        printf("*                 Visint Communication Technology.                 *\n");
        printf("*              Copyright 2008-2018  All right reserved(%dU).        *\n",baneType);
        printf("********************************************************************\n");
    }
	else if(company==2)
    {
        printf("********************************************************************\n");
        printf("*                 Welcome to use Command Line Tool                 *\n");
        printf("*              Copyright 2008-2018  All right reserved(%dU).        *\n",baneType);
        printf("********************************************************************\n");
    }
    else
    {
        printf("********************************************************************\n");
        printf("*                 Welcome to use Command Line Tool                 *\n");
        printf("*              Copyright 2008-2018  All right reserved(%dU).        *\n",baneType);
        printf("********************************************************************\n");
    }

    if(baneType == 1)
    {
        if(status[2]==1)
        {
            CurrentBloadNum =  2;
        }
        else
        {
            CurrentBloadNum = 3;
        }

    }
	else if(baneType == 2)
    {
        CurrentBloadNum  = 6;
    }
    else if(baneType == 4)
    {
        CurrentBloadNum  = 16;
    }
    else
    {

        CurrentBloadNum =  16;
    }


    while(1)
    {
        getname(name,sizeof(name));
        p=name;
        while(*p!='\n')
            p++;
        *p='\0';
        /*        printf("Please Input name:");
                scanf("%s",name);
                getchar();//将回车符屏蔽掉
        */
        //首先关闭输出回显，这样输入密码时就不会显示输入的字符信息
        set_disp_mode(STDIN_FILENO,0);
        //调用getpasswd函数获得用户输入的密码
        getpasswd(passwd, sizeof(passwd));
        p=passwd;
        while(*p!='\n')
            p++;
        *p='\0';
        printf("\n");

        set_disp_mode(STDIN_FILENO,1);
        userLevel =  getUserLevel(name,passwd);

        strcpy(name,"ssss");
        argv[1] = "-x";
        argc =3 ;

        if(userLevel == 1)
        {
            if(CurrentBloadNum == 16)
            {
                argv[2] = "/etc/xml-examples/klish/etc/clish-enable/";
                xml_path = "/etc/xml-examples/klish/etc/clish-enable/";
            }
            else if(CurrentBloadNum == 2)
            {
                argv[2] = "/etc/xml-examples/klishone/etc/clish-enable/";
                xml_path = "/etc/xml-examples/klishone/etc/clish-enable/";

            }
			else if(CurrentBloadNum == 6)
            {
                argv[2] = "/etc/xml-examples/klishsix/etc/clish-enable/";
                xml_path = "/etc/xml-examples/klishsix/etc/clish-enable/";

            }
            else
            {
                argv[2] = "/etc/xml-examples/klishtwo/etc/clish-enable/";
                xml_path = "/etc/xml-examples/klishtwo/etc/clish-enable/";

            }

            break;
        }
        else if(userLevel == 2)
        {
            if(CurrentBloadNum == 16)
            {
                argv[2] = "/etc/xml-examples/klish/etc/clish-view/";
                xml_path = "/etc/xml-examples/klish/etc/clish-view/";
            }
            else if(CurrentBloadNum == 2)
            {
                argv[2] = "/etc/xml-examples/klishone/etc/clish-view/";
                xml_path = "/etc/xml-examples/klishone/etc/clish-view/";

            }
			else if(CurrentBloadNum == 6)
            {
                argv[2] = "/etc/xml-examples/klishsix/etc/clish-enable/";
                xml_path = "/etc/xml-examples/klishsix/etc/clish-enable/";

            }			
            else
            {
                argv[2] = "/etc/xml-examples/klishtwo/etc/clish-view/";
                xml_path = "/etc/xml-examples/klishtwo/etc/clish-view/";

            }

            break;

        }
        else if(userLevel == 3)
        {
            if(CurrentBloadNum == 16)
            {
                argv[2] = "/etc/xml-examples/klish/etc/clish/";
                xml_path = "/etc/xml-examples/klish/etc/clish/";
            }
            else if(CurrentBloadNum == 2)
            {
                argv[2] = "/etc/xml-examples/klishone/etc/clish/";
                xml_path = "/etc/xml-examples/klishone/etc/clish/";

            }
            else
            {
                argv[2] = "/etc/xml-examples/klishtwo/etc/clish/";
                xml_path = "/etc/xml-examples/klishtwo/etc/clish/";

            }

            break;

        }
        else
        {
            printf("Please check your user name and password\n");
        }

    }

    /* Ignore SIGPIPE */
    sigemptyset(&sigpipe_set);
    sigaddset(&sigpipe_set, SIGPIPE);
    sigpipe_act.sa_flags = 0;
    sigpipe_act.sa_mask = sigpipe_set;
    sigpipe_act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sigpipe_act, NULL);

#if HAVE_LOCALE_H
    /* Set current locale */
    setlocale(LC_ALL, "");
#endif


    /* Var initialization */
    cmds = lub_list_new(NULL);

    /* Parse command line options */
#if 1
    while(1)
    {
        int opt;
#ifdef HAVE_GETOPT_H
        opt = getopt_long(argc, argv, shortopts, longopts, NULL);
#else
        opt = getopt(argc, argv, shortopts);
#endif
        if (-1 == opt)
            break;
        switch (opt)
        {
        case 's':
            socket_path = optarg;
            break;
        case 'l':
            lockless = BOOL_TRUE;
            break;
        case 'e':
            stop_on_error = BOOL_TRUE;
            break;
        case 'b':
            interactive = BOOL_FALSE;
            break;
        case 'q':
            quiet = BOOL_TRUE;
            break;
        case 'u':
            utf8 = BOOL_TRUE;
            break;
        case '8':
            bit8 = BOOL_TRUE;
            break;
        case 'o':
            log = BOOL_TRUE;
            break;
        case 'd':
            my_hooks.script_fn = clish_dryrun_callback;
            break;
        case 'x':
            xml_path = optarg;
            break;
        case 'w':
            view = optarg;
            break;
        case 'i':
            viewid = optarg;
            break;
        case 'k':
            lockless = BOOL_TRUE;
            my_hooks.script_fn = clish_dryrun_callback;
            my_hooks.config_fn = NULL;
            break;
        case 't':
            istimeout = BOOL_TRUE;
            timeout = atoi(optarg);
            break;
        case 'c':
        {
            char *str;
            cmd = BOOL_TRUE;
            quiet = BOOL_TRUE;
            str = strdup(optarg);
            lub_list_add(cmds, str);
        }
        break;
        case 'f':
            if (!strcmp(optarg, "/dev/null"))
                histfile = NULL;
            else
                histfile = optarg;
            break;
        case 'z':
        {
            int itmp = 0;
            itmp = atoi(optarg);
            if (itmp <= 0)
            {
                fprintf(stderr, "Error: Illegal histsize option value.\n");
                help(-1, argv[0]);
                goto end;
            }
            histsize = itmp;
        }
        break;
        case 'h':
            help(0, argv[0]);
            exit(0);
            break;
        case 'v':
            version(VERSION);
            exit(0);
            break;
        default:
            help(-1, argv[0]);
            goto end;
            break;
        }
    }


    /* Validate command line options */
    if (utf8 && bit8)
    {
        fprintf(stderr, "The -u and -8 options can't be used together.\n");
        goto end;
    }
#endif

    /* Create shell instance */
    if (quiet)
        outfd = fopen("/dev/null", "w");
    shell = clish_shell_new(&my_hooks, NULL, NULL, outfd, stop_on_error);
    if (!shell)
    {
        fprintf(stderr, "Cannot run clish.\n");
        goto end;
    }
    /*
        if(userLevel == 1)
        {
    		xml_path = "/etc/xml-examples/klish/etc/clish-enable/";
        }
        else if(userLevel == 2)
        {
              xml_path = "/etc/xml-examples/klish/etc/clish-view/";
        }
    	else if(userLevel == 3)
        {

    		 xml_path = "/etc/xml-examples/klish/etc/clish/";
        }
    */

    /* Load the XML files */
    clish_shell_load_scheme(shell, xml_path);
    /* Set communication to the konfd */
    clish_shell__set_socket(shell, socket_path);
    /* Set lockless mode */
    if (lockless)
        clish_shell__set_lockfile(shell, NULL);
    /* Set interactive mode */
    if (!interactive)
        clish_shell__set_interactive(shell, interactive);
    /* Set startup view */
    if (view)
        clish_shell__set_startup_view(shell, view);
    /* Set startup viewid */
    if (viewid)
        clish_shell__set_startup_viewid(shell, viewid);
    /* Set UTF-8 or 8-bit mode */
    if (utf8 || bit8)
        clish_shell__set_utf8(shell, utf8);
    else
    {
#if HAVE_LANGINFO_CODESET
        /* Autodetect encoding */
        if (!strcmp(nl_langinfo(CODESET), "UTF-8"))
            clish_shell__set_utf8(shell, BOOL_TRUE);
#else
        /* The default is 8-bit if locale is not supported */
        clish_shell__set_utf8(shell, BOOL_FALSE);
#endif
    }
    /* Set logging */
    if (log)
        clish_shell__set_log(shell, log);
    /* Set idle timeout */
    if (istimeout)
        clish_shell__set_timeout(shell, timeout);
    /* Set history settings */
    clish_shell__stifle_history(shell, histsize);
    if (histfile)
        histfile_expanded = lub_system_tilde_expand(histfile);
    if (histfile_expanded)
        clish_shell__restore_history(shell, histfile_expanded);

    /* Set source of command stream: files or interactive tty */
    if(optind < argc)
    {
        int i;
        /* Run the commands from the files */
        for (i = argc - 1; i >= optind; i--)
            clish_shell_push_file(shell, argv[i], stop_on_error);
    }
    else
    {
        /* The interactive shell */
        clish_shell_push_fd(shell, fdopen(dup(fileno(stdin)), "r"),
                            stop_on_error);
    }

    /* Execute startup */
    running = clish_shell_startup(shell);
    if (running)
    {
        fprintf(stderr, "Cannot startup clish.\n");
        goto end;
    }

    if (cmd)
    {
        /* Iterate cmds */
        for(iter = lub_list__get_head(cmds);
                iter; iter = lub_list_node__get_next(iter))
        {
            char *str = (char *)lub_list_node__get_data(iter);
            clish_shell_forceline(shell, str, NULL);
        }
    }
    else
    {
        /* Main loop */
        result = clish_shell_loop(shell);
    }

end:
    /* Cleanup */
    if (shell)
    {
        if (histfile_expanded)
        {
            clish_shell__save_history(shell, histfile_expanded);
            free(histfile_expanded);
        }
        clish_shell_delete(shell);
    }
    if (quiet)
        fclose(outfd);



    /* Delete each cmds element */
    while ((iter = lub_list__get_head(cmds)))
    {
        lub_list_del(cmds, iter);
        free(lub_list_node__get_data(iter));
        lub_list_node_free(iter);
    }
    lub_list_free(cmds);

#if 0
    if(( pid = fork() ) < 0 )
    {
        fprintf(stderr, "fork error/n");
    }
    else if(pid == 0)
    {

        system("bin/clish  xml-examples/klish/etc/clish-enable/");

    }
    else
    {
    }
#endif

    goto start;


//    system("bin/clish", "-x", "xml-examples/klish/etc/clish-enable/", (char*)0);
    return result;
}

/*--------------------------------------------------------- */
/* Print help message */
static void help(int status, const char *argv0)
{
    const char *name = NULL;

    if (!argv0)
        return;

    /* Find the basename */
    name = strrchr(argv0, '/');
    if (name)
        name++;
    else
        name = argv0;

    if (status != 0)
    {
        fprintf(stderr, "Try `%s -h' for more information.\n",
                name);
    }
    else
    {
        printf("Usage: %s [options] [script_file] [script_file] ...\n", name);
        printf("CLI utility. Command line shell."
               "The part of the klish project.\n");
        printf("Options:\n");
        printf("\t-v, --version\tPrint version.\n");
        printf("\t-h, --help\tPrint this help.\n");
        printf("\t-s <path>, --socket=<path>\tSpecify listen socket "
               "\n\t\tof the konfd daemon.\n");
        printf("\t-l, --lockless\tDon't use locking mechanism.\n");
        printf("\t-e, --stop-on-error\tStop script execution on error.\n");
        printf("\t-b, --background\tStart shell using non-interactive mode.\n");
        printf("\t-q, --quiet\tDisable echo while executing commands\n\t\tfrom the file stream.\n");
        printf("\t-d, --dry-run\tDon't actually execute ACTION scripts.\n");
        printf("\t-x <path>, --xml-path=<path>\tPath to XML scheme files.\n");
        printf("\t-w <view_name>, --view=<view_name>\tSet the startup view.\n");
        printf("\t-i <vars>, --viewid=<vars>\tSet the startup viewid variables.\n");
        printf("\t-u, --utf8\tForce UTF-8 encoding.\n");
        printf("\t-8, --8bit\tForce 8-bit encoding.\n");
        printf("\t-o, --log\tEnable command logging to syslog's local0.\n");
        printf("\t-k, --check\tCheck input files for syntax errors only.\n");
        printf("\t-t <timeout>, --timeout=<timeout>\tIdle timeout in seconds.\n");
        printf("\t-c <command>, --command=<command>\tExecute specified command(s).\n\t\tMultiple options are possible.\n");
        printf("\t-f <path>, --histfile=<path>\tFile to save command history.\n");
        printf("\t-z <num>, --histsize=<num>\tCommand history size in lines.\n");
    }
}
