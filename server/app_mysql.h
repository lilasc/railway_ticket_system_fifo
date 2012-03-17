#define MAX_SCHEDULE_RESULT 100
//malloc && free is necessary
struct schedule_search_st{
	int schedule_id[MAX_SCHEDULE_RESULT];
};
struct current_schedule_st{
	int schedule_id;
	char beginning[50];
	char destination[50];
	char schedule_time[30];
	int duration;
	int ticket_left;
};
/* Database backend functions */
int  database_start(const char *name, const char *psw);
void database_end();
/* Log in functions*/
int log_in(const char *name, const char *psw);//search member
int register_mem(const char *name, const char *psw);//update member
/*search functions*/
int search_schedule_row_num(char *beginning,char *destination,char *stime,char *etime,struct schedule_search_st *dest); //return number corresponds to conditions
int search_schedule_row(int id,struct current_schedule_st *dest); //return the id of schedule and the result is kept in content

/*book functions*/

int book(int mid,int sid,int num); //update ticket_left in schedule and number in sale;

int get_info(char *cmd, char *out);//get info to fill the combo box

int search_schedule(const char *cmd, char *out);

int buy_ticket(char *sid, char *mname, char *number);

int write_log(int mid, int sid, int num);

int search_history(char *mname, char *out);
