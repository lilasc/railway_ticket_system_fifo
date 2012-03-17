#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "app_mysql.h"

static MYSQL my_connection;
static int dbconnected = 0;


//-1 connect error
int database_start(const char *name, const char *pwd) {

    if (dbconnected) return 1;

    mysql_init(&my_connection);
    if (!mysql_real_connect(&my_connection, "localhost", name, pwd, "railway_ticket_system", 3306, NULL, 0)) {
        fprintf(stderr, "Database connection failure: %d, %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        return -1;
    }
    dbconnected = 1;
    return 1;
} /* database start */

void database_end() {
    if (dbconnected) mysql_close(&my_connection);
    dbconnected = 0;
} /* database_end */


//failed -1
//success new id
int log_in(const char *name, const char *psw){
    MYSQL_RES *res_ptr;
    MYSQL_ROW mysqlrow;

    int res;
    char ns[50];
    char ps[50];
    char is[250];
    int new_id=-1;

    if(!dbconnected)return -1;

    mysql_escape_string(ns,name,strlen(name));
    mysql_escape_string(ps,psw,strlen(psw));
    sprintf(is,"SELECT id FROM member WHERE name = '%s' and password='%s'",ns,ps);
    res = mysql_query(&my_connection,is);
    if(res)
        return -1;
    else {
        res_ptr = mysql_use_result(&my_connection);
        if (res_ptr) {
            if ((mysqlrow = mysql_fetch_row(res_ptr))) {
                sscanf(mysqlrow[0], "%d", &new_id);
            }
            mysql_free_result(res_ptr);
        }
        if(new_id!=-1) return new_id;
        return -1;
    }
}
//-1 connect error
//0 already exits
// UNIQUE ...
int register_mem(const char *name, const char *psw){
    int res;
    char ns[50];
    char ps[50];
    char is[250];

    if(!dbconnected)return -1;

    mysql_escape_string(ns,name,strlen(name));
    mysql_escape_string(ps,psw,strlen(psw));
    sprintf(is, "INSERT INTO member(name,password) VALUES('%s', '%s')", ns,ps);
    res = mysql_query(&my_connection, is);
    if (res) {
        fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&my_connection), mysql_error(&my_connection));
        return 0;
    }
    return 1;
}



void update_sale(int mid,int sid,int num){
    char iis[250];

    sprintf(iis,"INSERT INTO sale(sid,mid,number, sale_time) VALUES(%d,%d,%d, now())",sid,mid,num);
    if(mysql_query(&my_connection,iis))fprintf(stderr,"INSERT_sale error: %s\n",mysql_error(&my_connection));
}
void update_schedule(int sid,int num){
    int res;
    char is[250];
    sprintf(is,"UPDATE schedule SET ticket_left=%d WHERE id=%d",num,sid);
    res=mysql_query(&my_connection,is);
    if(res)
	fprintf(stderr,"UPDATE_schedule error : %s\n",mysql_error(&my_connection));

}

int write_log(int mid, int sid, int num)
{
    FILE *log;
    time_t rawtime;
    char stime[30];

    if ((log = fopen("log.txt", "r")) == NULL) {
        log = fopen("log.txt", "w");
        fprintf(log, "%-30s %-10s %-10s %-10s\n", "Time", "Mid", "Sid", "Number");
        fclose(log);
        log = NULL;
    }
    if (log != NULL)
        fclose(log);
    log = fopen("log.txt", "a");
    if (log != NULL) {
        time(&rawtime);
        sprintf(stime, "%s", ctime(&rawtime));
        stime[strlen(stime) - 1] = '\0';
        fprintf(log, "%-30s %-10d %-10d %-10d\n", stime, mid, sid, num);
        fclose(log);
    }
    else
        fprintf(stderr, "write log error");
}

//-1 connect error
//0 ticket shortage
int book(int mid,int sid,int num){
    MYSQL_RES *res_ptr;
    MYSQL_ROW mysqlrow;

    int res;
    char is[250];
    int ticket_left=-1;
    if(!dbconnected)return -1;
    sprintf(is,"SELECT ticket_left FROM schedule WHERE id= %d",sid);
    res=mysql_query(&my_connection,is);
    if(res){
	fprintf(stderr,"SELECT_book error : %s\n",mysql_error(&my_connection));
	return -1;
    }else {
        res_ptr=mysql_use_result(&my_connection);
        if(res_ptr){
            if ((mysqlrow = mysql_fetch_row(res_ptr))) {
                sscanf(mysqlrow[0], "%d", &ticket_left);
                if(ticket_left<num) return -1;

                ticket_left-=num;
                mysql_free_result(res_ptr);
                update_sale(mid,sid,num);
                update_schedule(sid,ticket_left);
                write_log(mid, sid, num);
                return 1;
            }
        }
        return -1;
    }
}
//-1 cannect error
//-2 time select error????
int search_schedule_row_num(char *beginning,char *destination,char *stime,char *etime,struct schedule_search_st *dest)
{
    MYSQL_RES *res_ptr;
    MYSQL_ROW mysqlrow;

    int res;
    char bs[50];
    char ds[50];
    char ts1[50];
    char ts2[50];
    char is[250];
    int  schedule_num=0;
    int i=0;
    if(!dbconnected)return -1;
    memset(dest,0,sizeof(*dest));
    mysql_escape_string(bs,beginning,strlen(beginning));
    mysql_escape_string(ds,destination,strlen(destination));
    mysql_escape_string(ts1,stime,strlen(stime));
    mysql_escape_string(ts2,etime,strlen(etime));
    //if(strcmp(stime,etime)==0)return -2;
    sprintf(is,"SELECT id FROM schedule WHERE beginning='%s' and destination='%s' and time>='%s' and time<='%s'",bs,ds,ts1,ts2);
    res=mysql_query(&my_connection,is);
    if(res){
        fprintf(stderr,"SELECT error : %s\n",mysql_error(&my_connection));
        return -1;
    }
    else{
        res_ptr=mysql_store_result(&my_connection);
        if(res_ptr){
            if((schedule_num=mysql_num_rows(res_ptr))>0){
                while(mysqlrow=mysql_fetch_row(res_ptr)){
                    sscanf(mysqlrow[0],"%d",&dest->schedule_id[i]);
                    i++;
                }
                mysql_free_result(res_ptr);
                return schedule_num;
            }
            mysql_free_result(res_ptr);
            return 0;
        }
    }
    mysql_free_result(res_ptr);
    return -1;

}
int search_schedule_row(int id,struct current_schedule_st *dest){
    MYSQL_RES *res_ptr;
    MYSQL_ROW mysqlrow;

    int res;
    char is[250];
    if(!dbconnected)return -1;
    memset(dest,0,sizeof(*dest));

    sprintf(is,"SELECT id,beginning,destination, time, duration,ticket_left from schedule where id=%d",id);
    res=mysql_query(&my_connection,is);
    if(res){
        fprintf(stderr,"SELECT error : %s\n",mysql_error(&my_connection));
        return -1;
    }
    else{
        res_ptr=mysql_use_result(&my_connection);
        if(res_ptr){
            mysqlrow=mysql_fetch_row(res_ptr);
            sscanf(mysqlrow[0],"%d",&dest->schedule_id);
            sscanf(mysqlrow[4],"%d",&dest->duration);
            sscanf(mysqlrow[5],"%d",&dest->ticket_left);
            strcpy(dest->beginning,mysqlrow[1]);
            strcpy(dest->destination,mysqlrow[2]);
            strcpy(dest->schedule_time,mysqlrow[3]);

            mysql_free_result(res_ptr);
            return 1;
        }
        mysql_free_result(res_ptr);
        return -1;
    }

}


int search_schedule(const char *cmd, char *out)
{
    int res;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    int count = 0;
    int i;

    res=mysql_query(&my_connection,cmd);
    out[0] = '\0';

    if(res){
        fprintf(stderr,"SELECT error : %s\n",mysql_error(&my_connection));
        return -1;
    }
    else{
        res_ptr=mysql_use_result(&my_connection);

        if (res_ptr) {
            while ((sqlrow = mysql_fetch_row(res_ptr))) {
                //7 is the column count
                for (i = 0; i < 7; i++) {
                    strcat(out, sqlrow[i]);
                    strcat(out, ";");
                }
                count++;
            }
            mysql_free_result(res_ptr);
        }
        return count;
    }
}

int buy_ticket(char *sid, char *mname, char *number)
{
    char cmd[512];
    char mid[128];


    sprintf(cmd, "select id from member where name='%s'", mname);
    get_info(cmd, mid);
    return book(atoi(mid), atoi(sid), atoi(number));
}

int get_info(char *cmd, char *out)
{
    int res;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    int count = 0;

    res=mysql_query(&my_connection,cmd);
    out[0] = '\0';

    if(res){
        fprintf(stderr,"SELECT error : %s\n",mysql_error(&my_connection));
        return -1;
    }
    else{
        res_ptr=mysql_use_result(&my_connection);

        if (res_ptr) {
            while ((sqlrow = mysql_fetch_row(res_ptr))) {
                strcat(out, sqlrow[0]);
                strcat(out, ";");
                count++;
            }
            mysql_free_result(res_ptr);
        }
        return count;
    }
}

int search_history(char *mname, char *out)
{
    char cmd[512];
    char mid[128];
    int res;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    int count = 0;
    int i;

    sprintf(cmd, "select id from member where name='%s'", mname);
    get_info(cmd, mid);

    sprintf(cmd, "select sid, number, sale_time from sale where mid='%s'", mid);

    res=mysql_query(&my_connection,cmd);
    out[0] = '\0';

    if(res){
        fprintf(stderr,"SELECT error : %s\n",mysql_error(&my_connection));
        return -1;
    }
    else{
        res_ptr=mysql_use_result(&my_connection);

        if (res_ptr) {
            while ((sqlrow = mysql_fetch_row(res_ptr))) {
                //7 is the column count
                for (i = 0; i < 3; i++) {
                    strcat(out, sqlrow[i]);
                    strcat(out, ";");
                }
                count++;
            }
            mysql_free_result(res_ptr);
        }
        return count;
    }
}
