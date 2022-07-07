#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<termios.h>
#include<unistd.h>

#define max_days 14
#define max_dates 31		// today + 30 days later
#define min_ppl 8
#define max_ppl 128
#define max_room 12
#define max_table 16
#define max_ppl_per_table 12

struct customer                                         //define the structure customer
{
	char cID[5];
	char name[30];
	int dob;
	int contact;
	char email[254];                                //maximum length limit of an email address is 254
	char pw[30];
};

typedef struct customer customer_t;                     //define a type with structure customer

struct booking
{
   	char bookID[5];
   	char cID[5];
    	int checkin_date;
   	int checkout_date;
  	int ppl;
    	int room_cost;
        int meal_cost;
        int new_cost;           //in case refund or pay the difference, NULL for new booking
        char pay_status[10];    //payment status: complete or pending.
};

typedef struct booking booking_t;

struct room
{
        char roomID[5];
        int date;
        int type;
        char bookID[5];
};

typedef struct room room_t;

struct canteen
{
        char tableID[5];
        int date;
        char period[4];
        char meal_code[2];
        int quantity;
        char bookID[5];

};

typedef struct canteen canteen_t;

struct payment
{
        char payID[5];
        char bookID[5];
        int fee;
        char method[20];
        char status[20];
};

typedef struct payment payment_t;

char cID[5];
int total_customer;
customer_t customer[100];

int total_booking;
booking_t booking[500];
booking_t soon_booking[500];

int total_payment;
payment_t payment[500];

char room_id[max_room][5]={"r041", "r042", "r043", "r044", "r045", "r046",  "r121", "r122", "r123", "r124", "r281", "r282"};
int room_type[max_room] = {4,4,4,4,4,4,12,12,12,12,28,28};
room_t room[max_dates*max_room];

int rm4, rm12, rm28;
int start, end, days, ppl;
int room_ok_start[max_dates], room_ok_end[max_dates];                   //array of start and corresponding end dates with rooms available
int room_ok_start_i[max_dates];                                         //array of the index of the checkin dates in dates[max_dates]
int no_of_dates_room_ok;                                                             //no. of dates with rooms available

char table_id[max_table][5];
char period[4] = {"BLD"};
canteen_t canteen[max_dates*max_table*3];

int checked_room_availability;
int complete;
int amend;
int logged_in;

int today;
time_t now;
int dates[max_dates];

void read_customer();
void write_customer();
void read_booking();
void write_booking();
void read_room();
void write_room();
void read_canteen();
void write_canteen();
void read_payment();
void write_payment();

void set_today();
void dates_included();
void update_room_dates();
void update_canteen_dates();
int range_check(char str[], int min, int max);

void booking_();
void log_in();
void register_();
void reset_password();
void display_past_booking();

void read_preference();
int check_room_availability(int start_i, int end_i);
void reserve_room();
void order_meal(int booking_i);
void confirm_booking();
void show_booking_customer(int booking_i);

void complete_payment();
int verify_payment();

void amend_booking(int max_soon);
void cancel_booking(int max_soon);

void staff_log_in();
void show_booking_staff();
void show_room();
void show_canteen();

void read_customer()
{
        int i;
	FILE *fp;

        fp = fopen("customer.txt", "r");

        i=0;

		fscanf(fp, "%s", customer[i].cID);
        fscanf(fp, "%s", customer[i].name);
        fscanf(fp, "%d", &customer[i].dob);
        fscanf(fp, "%s", customer[i].pw);
        fscanf(fp, "%d", &customer[i].contact);
        fscanf(fp, "%s", customer[i].email);

        while(!feof(fp))
        {
                i++;
                fscanf(fp, "%s", customer[i].cID);
                fscanf(fp, "%s", customer[i].name);
                fscanf(fp, "%d", &customer[i].dob);
                fscanf(fp, "%s", customer[i].pw);
                fscanf(fp, "%d", &customer[i].contact);
                fscanf(fp, "%s", customer[i].email);
        }

	total_customer = i;

        fclose(fp);
}

void write_customer()
{
        FILE *write;
        int i;

        write = fopen("customer.txt", "w");                     //writing new customer data into the customer.txt file

        for(i=0; i<total_customer; i++)
                fprintf(write, "%s \t%s \t%d \t%s \t%d \t%s \n", customer[i].cID, customer[i].name, customer[i].dob, customer[i].pw, customer[i].contact, customer[i].email);

        fclose(write);
}

void read_booking()
{
        int i;

        FILE *read;

        read = fopen("booking.txt", "r");

        i=0;
		fscanf(read, "%s", booking[i].bookID);
        fscanf(read, "%s", booking[i].cID);
        fscanf(read, "%d", &booking[i].checkin_date);
        fscanf(read, "%d", &booking[i].checkout_date);
        fscanf(read, "%d", &booking[i].ppl);
        fscanf(read, "%d", &booking[i].room_cost);
        fscanf(read, "%d", &booking[i].meal_cost);
        fscanf(read, "%d", &booking[i].new_cost);
        fscanf(read, "%s", booking[i].pay_status);

        while(!feof(read))
        {
                i++;
                fscanf(read, "%s", booking[i].bookID);
                fscanf(read, "%s", booking[i].cID);
                fscanf(read, "%d", &booking[i].checkin_date);
                fscanf(read, "%d", &booking[i].checkout_date);
                fscanf(read, "%d", &booking[i].ppl);
                fscanf(read, "%d", &booking[i].room_cost);
	        	fscanf(read, "%d", &booking[i].meal_cost);
                fscanf(read, "%d", &booking[i].new_cost);
                fscanf(read, "%s", booking[i].pay_status);
        }

	fclose(read);

        total_booking = i;
}

void write_booking()
{
        FILE *write;
        int i;

        write = fopen("booking.txt", "w");

        for(i=0; i<total_booking; i++)
                fprintf(write, "%s \t%s \t%d \t%d \t%d \t%d \t%d \t%d \t%s \n", booking[i].bookID, booking[i].cID, booking[i].checkin_date, booking[i].checkout_date, booking[i].ppl, booking[i].room_cost, booking[i].meal_cost, booking[i].new_cost, booking[i].pay_status);

        fclose(write);
}

void read_room()
{
        FILE *read;
        int i=0;

        read = fopen("room.txt", "r");

        fscanf(read, "%s", room[i].roomID);
        fscanf(read, "%d", &room[i].date);
        fscanf(read, "%d", &room[i].type);
        fscanf(read, "%s", room[i].bookID);

        while(!feof(read))
        {
                i++;
                fscanf(read, "%s", room[i].roomID);
                fscanf(read, "%d", &room[i].date);
                fscanf(read, "%d", &room[i].type);
                fscanf(read, "%s", room[i].bookID);
        }

        fclose(read);
}

void write_room()
{
        FILE *write;
        int i;

        write = fopen("room.txt", "w");

        for(i=0; i<max_room*max_dates; i++)
                fprintf(write, "%s \t%d \t%d \t%s \n", room[i].roomID, room[i].date, room[i].type, room[i].bookID);

        fclose(write);
}

void read_canteen()
{
        FILE *read;
        int i;

        i=0;
	read = fopen("canteen.txt", "r");

        fscanf(read, "%s", canteen[i].tableID);
        fscanf(read, "%d", &canteen[i].date);
        fscanf(read, "%s", canteen[i].period);
        fscanf(read, "%s", canteen[i].meal_code);
        fscanf(read, "%d", &canteen[i].quantity);
        fscanf(read, "%s", canteen[i].bookID);

        while(!feof(read))
        {
                i++;
                fscanf(read, "%s", canteen[i].tableID);
                fscanf(read, "%d", &canteen[i].date);
                fscanf(read, "%s", canteen[i].period);
                fscanf(read, "%s", canteen[i].meal_code);
                fscanf(read, "%d", &canteen[i].quantity);
                fscanf(read, "%s", canteen[i].bookID);
        }

        fclose(read);
}

void write_canteen()
{
        FILE *write;
        int i;

        write = fopen("canteen.txt", "w");

        for(i=0; i<max_dates*3*max_table; i++)
                fprintf(write, "%s \t%d \t%s \t%s \t%d \t%s \n", canteen[i].tableID, canteen[i].date, canteen[i].period, canteen[i].meal_code, canteen[i].quantity, canteen[i].bookID);

        fclose(write);
}

void read_payment()
{
        FILE *read;
        int i=0;

        read = fopen("payment.txt", "r");

        fscanf(read, "%s", payment[i].payID);
        fscanf(read, "%s", payment[i].bookID);
        fscanf(read, "%d", &payment[i].fee);
        fscanf(read, "%s", payment[i].method);
        fscanf(read, "%s", payment[i].status);

        while(!feof(read))
        {
                i++;
                fscanf(read, "%s", payment[i].payID);
                fscanf(read, "%s", payment[i].bookID);
                fscanf(read, "%d", &payment[i].fee);
                fscanf(read, "%s", payment[i].method);
                fscanf(read, "%s", payment[i].status);
        }

	total_payment = i;

        fclose(read);
}

void write_payment()
{
        FILE *write;
        int i;

        write = fopen("payment.txt", "w");

        for(i=0; i<total_payment; i++)
                fprintf(write, "%s \t%s \t%d \t%s \t%s \n", payment[i].payID, payment[i].bookID, payment[i].fee, payment[i].method, payment[i].status);

        fclose(write);
}

void set_today()                                         //obtain the today's date from time library
{
        char date[10];
        struct tm* info;

        now = time(NULL);
        info = localtime(&now);

        strftime(date, 10, "%Y%m%d", info);
        today = atoi(date);
}

void date_included()                                    //store all the dates for booking to the array
{
        int i;
	struct tm* info;
        time_t day;
        char day_s[10];

        for(i=0; i<=max_dates; i++)
        {
                day = now + 86400 * i;
                info = localtime(&day);
                strftime(day_s, 10, "%Y%m%d", info);
                dates[i] = atoi(day_s);
        }
}

void update_room_dates()
{
        FILE *read, *write;
        int i=0, j, k;
        room_t check[max_dates*max_room];

        read = fopen("room.txt", "r");

        fscanf(read, "%s", check[i].roomID);
        fscanf(read, "%d", &check[i].date);
        fscanf(read, "%d", &check[i].type);
        fscanf(read, "%s", check[i].bookID);

        while(!feof(read))
        {
                i++;
                fscanf(read, "%s", check[i].roomID);
                fscanf(read, "%d", &check[i].date);
                fscanf(read, "%d", &check[i].type);
                fscanf(read, "%s", check[i].bookID);
        }

	fclose(read);

        if(check[0].date!=today)
        {
                write = fopen("room.txt","w");

                i=0;
                while(check[i].date!=today && i<max_dates)
                        i++;

                if(i==max_dates)
                {
                        for(j=0; j<max_room;j++)
                                for(k=0; k<max_dates; k++)
                                        fprintf(write, "%s \t%d \t%d \t%s \n", room_id[j], dates[k], room_type[j], "NULL");
                }
                else if(i<max_dates)
                {
                        for(j=0; j<max_room; j++)
                        {
                                for(k=i; k<max_dates; k++)
                                        fprintf(write, "%s \t%d \t%d \t%s \n", check[max_dates*j+k].roomID, check[max_dates*j+k].date, check[max_dates*j+k].type, check[max_dates*j+k].bookID);
                                for(k=max_dates-i; k<max_dates; k++)
                                        fprintf(write, "%s \t%d \t%d \t%s \n", room_id[j], dates[k], room_type[j], "NULL");
                        }
                }

                fclose(write);
        }
}

void update_canteen_dates()
{
        FILE *read, *write;
        int i, j, k, m;
        canteen_t check[max_dates*max_table*3];

        for(i=1; i<=max_table; i++)
                sprintf(table_id[i-1], "t%d", i);

	i=0;
	read = fopen("canteen.txt", "r");

        fscanf(read, "%s", check[i].tableID);
        fscanf(read, "%d", &check[i].date);
        fscanf(read, "%s", check[i].period);
        fscanf(read, "%s", check[i].meal_code);
        fscanf(read, "%d", &check[i].quantity);
        fscanf(read, "%s", check[i].bookID);

        while(!feof(read))
        {
                i++;
                fscanf(read, "%s", check[i].tableID);
                fscanf(read, "%d", &check[i].date);
                fscanf(read, "%s", check[i].period);
                fscanf(read, "%s", check[i].meal_code);
                fscanf(read, "%d", &check[i].quantity);
                fscanf(read, "%s", check[i].bookID);
        }

        fclose(read);

        if(check[0].date!=today)
        {
                write = fopen("canteen.txt", "w");

                i=0;
                while(check[i].date!=today && i<max_dates*max_table*3)
                        i=i+max_table*3;

                if(i==max_dates*max_table*3)                                                   //regenerate a new canteen.txt files with the records of today and the next 30 days
                {
                        for(j=0; j<max_dates; j++)
                                for(k=0; k<3; k++)
                                        for(m=0; m<max_table; m++)
                                                fprintf(write, "%s \t%d \t%c \t%s \t%d \t%s \n", table_id[m], dates[j], period[k], "-", 0, "NULL");
                }
                else if(i<max_dates*max_table*3)                                               //delete the records that was before today
                {
                        for(j=i; j<max_dates*3*max_table; j++)
                                fprintf(write, "%s \t%d \t%s \t%s \t%d \t%s \n", check[j].tableID, check[j].date, check[j].period, check[j].meal_code, check[j].quantity, check[j].bookID);
                        for(j=max_dates-i/(3*max_table); j<max_dates; j++)
                                for(k=0; k<3; k++)                                              //add the new dates to the record
                                        for(m=0; m<max_table; m++)
                                                fprintf(write, "%s \t%d \t%c \t%s \t%d \t%s \n", table_id[m], dates[j], period[k], "-", 0, "NULL");
                }

                fclose(write);
        }
}

int range_check(char str[], int min, int max)
{
        int within=1, i, x;
        char check;

        for(i=0;i<strlen(str); i++)
        {
                check = str[i];
                if(check<48 || check>57)
                        within = 0;
        }
	x = atoi(str);
        if(x<min || x>max)
                within=0;

        return within;
}
	
void booking_()
{
        int choice;
        char temp, str[100];

        read_booking();
        read_canteen();
        read_payment();
        read_room();

        if(logged_in==0)
        {
                printf("\n[1] Login");
                printf("\n[2] Forget Password");
                printf("\n[3] Register\n\n");

                do{
                   	printf("Please select an action : ");
                        scanf("%s%c", str, &temp);
                }while(range_check(str, 1, 3) == 0);
		choice = atoi(str);

                if(choice == 1)
                        log_in();
                else if(choice == 2)
                        reset_password();
                else if(choice == 3)
                        register_();
        }
	else if(logged_in==1)
                display_past_booking();

        write_booking();
        write_room();
        write_canteen();
        write_payment();
}

void log_in()
{
        char name[30], pw[30], temp, str[100];
        int dob,i, matched, error_choice;

        struct termios term, term_orig;
        tcgetattr(STDIN_FILENO, &term);
        term_orig = term;
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);

        read_customer();

        printf("\n              <<LOG IN PAGE>>\n");
        do{
           	printf("\nName(e.g AppleChan) : ");
                fgets(name, 30, stdin);
                name[strlen(name)-1] = '\0';

                do{
                   	printf("D.O.B. (YYYYMMDD) : ");
                        scanf("%s%c", str, &temp);
                }while(range_check(str, 19000101, today)==0);
		dob = atoi(str);	

                printf("Password : ");
                tcsetattr(STDIN_FILENO, TCSANOW, &term);        //disabling the input echo function to hide the password
                fgets(pw, 30, stdin);
                pw[strlen(pw)-1] = '\0';

                tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);   //enabling the input echo function

                i=0;
                while(!(dob==customer[i].dob && strcmp(name,customer[i].name)==0 && strcmp(pw, customer[i].pw)==0) && i<total_customer)
                        i++;

                if(i==total_customer)
                {
                        matched = 0;
                        printf("\n\nRecord not found. (1)Try again, (2)Register or (3)Reset password?\n");
                        do{
                           	printf("Please choose your action : ");
                                scanf("%s%c", str, &temp);
                        }while(range_check(str, 1, 3)==0);
			error_choice = atoi(str);
                }
                else if(i<total_customer)
                        matched = 1;
        }while(matched == 0 && error_choice == 1);

        strcpy(cID, customer[i].cID);

        if(matched == 1)
        {
                logged_in = 1;
                if(checked_room_availability == 1)
                        reserve_room();
                else
                    	display_past_booking();
        }
	else if(error_choice==2)
                register_();
        else if(error_choice==3)
                reset_password();

}

void register_()
{
        char name[30], pw1[100], pw2[30], email[254], temp, str[100], str2[100], str3[100];
        int i, j, dob, contact, yyyy, mm, dd, dup_choice, valid_date, isleap;

        struct termios term, term_orig;
        tcgetattr(STDIN_FILENO, &term);
        term_orig = term;
        term.c_lflag &= ~ECHO;

        read_customer();

        printf("\n              <<REGISTRATION>>\n");
        printf("Please fill in your personal informations.\n\n");

        do{
           	printf("Name (e.g AppleChan) : ");
                fgets(name, 30, stdin);
                name[strlen(name)-1] = '\0';

                i=0;
                while(i<strlen(name) && name[i] != ' ')
                        i++;
                if(i<strlen(name))
                        printf("Please type your name in the correct format. (without space)\n");
        }while(i<strlen(name));

       	do{
		do{
              		printf("D.O.B. (YYYY MM DD) : ");
               		scanf("%s %s %s%c", str, str2, str3, &temp);
       		}while(range_check(str, 1900, today/10000)==0 || range_check(str2, 1, 12)==0 || range_check(str3, 1, 31)==0);
		yyyy = atoi(str);
		mm = atoi(str2);
		dd = atoi(str3);
		
		valid_date=0;
		if (mm==1 || mm==3 || mm==5 || mm==7 || mm==8 || mm==10 || mm==12)
		{
			if (dd<=31 && dd>=1)
				valid_date = 1;
		}
		else if (mm==4 || mm==6 || mm==9 || mm==11)
		{
			if(dd<=30 && dd>=1)
				valid_date = 1;
		}
		else if (mm==2)
		{
			if(yyyy%4 != 0)				//common year
				isleap = 0;
			else if(yyyy%100 != 0)			//leap year
				isleap = 1;
			else if(yyyy%400 != 0)			//common year
				isleap = 0;
			else					//leap year
				isleap = 1;

			if(isleap == 1)
			{
				if(dd>=1 && dd<=29)
					valid_date = 1;
			}
			else if(isleap == 0)
			{
				if(dd>=1 && dd<=28)
					valid_date = 1;
			}
		}
       		dob = yyyy*10000 + mm*100 + dd;					
	}while(dob<19000101 || dob>today || valid_date==0);

        do{
           	printf("Contact no. : ");
                scanf("%s%c", str, &temp);
        }while(range_check(str, 10000000, 99999999)==0);
	contact = atoi(str);

        do{
           	printf("Email : ");
                fgets(email, 254, stdin);
                email[strlen(email)-1] = '\0';

                i=0;                                                    //check whether the email input include space
                while(i<strlen(email) && email[i] != ' ')
                        i++;

                j=0;
                while(j<strlen(email) && email[j] != '@')               //check whether the email input include '@'
                        j++;

        }while(i<strlen(email) || j == strlen(email));

        printf("\n");

        i=0;                                                            //check whether the user has registered already
        while(i<total_customer && (strcmp(name, customer[i].name)!=0 && dob!=customer[i].dob && contact!=customer[i].contact))
                i++;

        if(i<total_customer)
        {
                printf("You have already registered. You may choose to (1)Log in or (2)Reset your password.\n");
                do{
                   	printf("Please choose your action : ");
                        scanf("%s%c", str, &temp);
                }while(range_check(str, 1, 2)==0);
		dup_choice = atoi(str);

                if(dup_choice == 1)
                        log_in();
                else if(dup_choice == 2)
                        reset_password();
        }
	else if (i==total_customer)
        {
                tcsetattr(STDIN_FILENO, TCSANOW, &term);                        //disabling the input echo function to hide the password

                do{
                   	printf("Set your password : ");
                        fgets(pw1, 100, stdin);
                        printf("\n");
                        pw1[strlen(pw1)-1] = '\0';

                        if(strlen(pw1)<8)
                                printf("The password is too short.\n");
                        else if(strlen(pw1)>30)
                                printf("The password is too long. \n");

                }while(strlen(pw1)<8 || strlen(pw1)>30);

                do{
                   	printf("Enter password again : ");
                        fgets(pw2, 30, stdin);
                        pw2[strlen(pw2)-1] = '\0';
                        printf("\n");

                        if(strcmp(pw1, pw2) != 0)
                                printf("Mismatched password. Please try again.\n");

                }while(strcmp(pw1, pw2) != 0);

                tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);           //enabling the input echo function

                strcpy(customer[total_customer].name, name);
                customer[total_customer].dob = dob;
                strcpy(customer[total_customer].pw, pw1);
                customer[total_customer].contact = contact;
                strcpy(customer[total_customer].email, email);
                sprintf(customer[total_customer].cID, "c%d", total_customer+1);

                total_customer += 1;

                write_customer();

                printf("\nRegistration completed. Please verify your account through your email and log in again to start your booking.\n");

                log_in();
        }

}

void reset_password()
{
        char name[30], pw1[30], pw2[30], temp, str[100];
        int dob,i, matched, error_choice, contact;

        struct termios term, term_orig;
        tcgetattr(STDIN_FILENO, &term);
        term_orig = term;
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);

        set_today();

        read_customer();

        printf("\n              <<RESET PASSWORD>>\n");

        do{
           	printf("\nName(e.g AppleChan) : ");
                fgets(name, 30, stdin);
                name[strlen(name)-1] = '\0';

                do{
                   	printf("D.O.B. (YYYYMMDD) : ");
                        scanf("%s%c", str, &temp);
                }while(range_check(str, 19000101, today)==0);
		dob = atoi(str);

                do{
                   	printf("Contact no. : ");
                        scanf("%s%c", str, &temp);
                }while(range_check(str, 10000000, 99999999)==0);
		contact = atoi(str);

                i=0;
                while(!(dob==customer[i].dob && strcmp(name,customer[i].name)==0 && contact==customer[i].contact) && i<total_customer)
                        i++;

                if(i==total_customer)
                {
                        matched = 0;
                        printf("\nRecord not found. Please (1)Try again or (2)Register a new account.\n");
                        do{
                           	printf("Please choose your action : ");
                                scanf("%s%c", str, &temp);
                        }while(range_check(str, 1, 2)==0);
			error_choice = atoi(str);
                }
                else if(i<total_customer)
                {
                        matched = 1;
                }
        }while(matched == 0 && error_choice == 1);

        if(matched == 1)
        {
                tcsetattr(STDIN_FILENO, TCSANOW, &term);                        //disabling the input echo function to hide the password

                do{
                   	printf("Reset your password : ");
                        fgets(pw1, 30, stdin);
                        printf("\n");
                        pw1[strlen(pw1)-1] = '\0';

                        if(strlen(pw1)<8)
                                printf("The password is too short.\n");
                        else if(strlen(pw1)>30)
                                printf("The password is too long. \n");

                }while(strlen(pw1)<8 || strlen(pw1)>30);

                do{
                   	printf("Enter password again : ");
                        fgets(pw2, 30, stdin);
                        pw2[strlen(pw2)-1] = '\0';
                        printf("\n");

                        if(strcmp(pw1, pw2) != 0)
                                printf("Mismatched password. Please try again.\n");

                }while(strcmp(pw1, pw2) != 0);

                tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);           //enabling the input echo function

                strcpy(customer[i].pw, pw1);

                write_customer();

                printf("Your password is reset.\n");

                log_in();
        }
	else if(error_choice==2)
                register_();
}

void display_past_booking()
{
        int i,j=0, k=0, choice;
        char temp, str[100];

        printf("\nWelcome back to our website!\n");

        printf("\n		<<YOUR PAST BOOKING RECORDS>>\n\n");
        printf("    	Booking ID	Checkin	date	Checkout date	Payment		Status\n\n");
        for(i=0; i<total_booking; i++)
                if(strcmp(booking[i].cID, cID)==0)
                {
                        k++;
                        printf("[%d] \t%s \t\t%d \t%d \t%s \t", k, booking[i].bookID, booking[i].checkin_date, booking[i].checkout_date, booking[i].pay_status);

			if(strcmp(booking[i].pay_status, "cancelled")==0)
				printf("cancelled\n");
                        else if(booking[i].checkin_date>today)
                        {
                                printf("soon\n");
                               	soon_booking[j] = booking[i];
                                j++;
			}
                        else if(booking[i].checkout_date<today)
                                printf("finished\n");
                        else
                            	printf("happening\n");

                }
	
	printf("\n");
	if(j>0)
	{
           	do{
			printf("Would you like to (1)Add New Booking, (2)Amend Booking, (3)Cancel Booking, (4)Log out or (5)Back to Main Menu : ");
                	scanf("%s%c", str, &temp);
        	}while(range_check(str, 1, 5)==0);
		choice = atoi(str);
	}
	else if(j==0)													//no booking to amend nor cancel
	{
           	do{
			do{
                        	printf("Would you like to (1)Add New Booking, (4)Log out or (5)Back to Main Menu : ");
                        	scanf("%s%c", str, &temp);
			}while(range_check(str, 1, 5)==0);
			choice = atoi(str);
                }while(choice!=1 && choice!=4 && choice!=5);		
	}

        if(choice==1)
	{
		amend = 0;
                read_preference();
        }
	else if(choice==2)
	{
		amend = 1;
	      	amend_booking(j);
	}
	else if(choice==3)
                cancel_booking(j);
        else if(choice==4)
                logged_in = 0;
}

void read_preference()
{
	int within, rmok, start_i, end_i;
	int i, j, diff, available, choice;
	char temp, str[100];

	do{
		printf("Please fill in your booking preference.\n\n");

		printf("[REMINDER]You can only book camp of the upcoming %d days. (From %d to %d)\n", max_dates, dates[1], dates[max_dates-1]);	
		printf("[REMINDER]You should have at least %d participants for one booking.\n", min_ppl);
		printf("[REMINDER]The number of days should not exceed %d.\n\n", max_days);

		printf("[1] Camping period preferred inclusively(the period you are free)\n");

		do{
			do{
				printf("    Starting date (YYYYMMDD) : ");
				scanf("%s%c", str, &temp);
				if(range_check(str, dates[1], dates[max_dates-2])==0)
					printf("    [REMINDER]You can only book camp of the upcoming %d days. (From %d to %d)\n", max_dates, dates[1], dates[max_dates-1]);
			}while(range_check(str, dates[1], dates[max_dates-2])==0);
			start = atoi(str);

			start_i=1;					//check whether the start date is in the 30days later.
			while(start_i<max_dates && start!=dates[start_i])
				start_i++;

			if(start_i==max_dates || start_i==max_dates-1)
			{
				within=0;
				printf("    [REMINDER]You can only book camp of the upcoming %d days. (From %d to %d)\n", max_dates, dates[1], dates[max_dates-1]);
			}
			else
				within=1;
		}while(within==0);

		do{
			do{
				printf("    Ending date (YYYYMMDD) : ");
				scanf("%s%c", str, &temp);
				if(range_check(str, dates[2], dates[max_dates-1])==0)
					printf("    [REMINDER]You can only book camp of the upcoming %d days. (From %d to %d)\n", max_dates, dates[1], dates[max_dates-1]);
			}while(range_check(str, dates[2], dates[max_dates-1])==0);
			end = atoi(str);

			end_i=2;
			while(end_i<max_dates && end!=dates[end_i])
				end_i++;
		
			if(end_i==max_dates)
			{
                        	within=0;
                        	printf("    [REMINDER]You can only book camp of the upcoming %d days. (From %d to %d)\n", max_dates, dates[0], dates[max_dates-1]);
                	}
                	else
                    		within=1;

			diff = end_i - start_i;
	
		}while(within==0 || diff<=0);

		do{
			do{
				printf("[2] Number of camping days : ");
				scanf("%s%c", str, &temp);
			}while(range_check(str, 1, max_days)==0);
			days = atoi(str);
		}while(days>diff);

		do{
			printf("[3] Number of participants : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str,	min_ppl, max_ppl)==0);
		ppl = atoi(str);		

		printf("[4] We provide 4-people Room($400, max:6), 12-people Room($1200, max:4) and 28-people Room($2800, max:2).\n");
		do{
			printf("    Quantities of each room(s) you want to book : \n");

			do{
				printf("    ~ 4-people Room	: ");
				scanf("%s%c",str, &temp);
			}while(range_check(str, 0, 6)==0);
			rm4 = atoi(str);
	
                	do{
                        	printf("    ~ 12-people Room   	: ");
                        	scanf("%s%c",str, &temp);
                        }while(range_check(str, 0, 4)==0);
                        rm12 = atoi(str);

                	do{
                        	printf("    ~ 28-people Room  	: ");
                        	scanf("%s%c",str, &temp);
                        }while(range_check(str, 0, 2)==0);
                        rm28 = atoi(str);

			rmok = 0;
			if(rm4+rm12+rm28<1)
				printf("    Please book at least one room.\n");
			else if(rm4*4 + rm12*12 + rm28*28 < ppl)
				printf("    Please book more rooms to accommodate all participants.\n");
			else
				rmok = 1;		
		}while(rmok==0);

		available = check_room_availability(start_i, end_i);

		if(available == 1)
		{
			printf("\n");
			do{
				printf("Please choose your action, (0)Check other preference, (1)Book Now or (2)Back to Main Menu : ");
				scanf("%s%c", str, &temp);
			}while(range_check(str, 0, 2)==0);
			choice = atoi(str);

			if(choice == 1)
			{
				checked_room_availability = 1;
				if(logged_in == 0)
					log_in();
				else if(logged_in == 1)
					reserve_room();
			}
		}
		else if(available == 0)
		{
			printf("\nWe are sorry that there are no matched rooms available in this period.\n");
			do{
				printf("Pleae choose your action, (0)Check other preference or (1)Back to Main Menu : ");
				scanf("%s%c", str, &temp);
			}while(range_check(str, 0, 1)==0);
			choice = atoi(str);
		}
	}while(choice==0);
}

int check_room_availability(int start_i, int end_i) 						//declare function
{
	int i=0, j;	
	int ok4[max_dates], ok12[max_dates], ok28[max_dates], temp[max_dates];
	int a, b, c, ad4rm, arms4d;					//ad4rm = available days in the specific period for this room; arms4d = avaialble rooms in this period starting from this day
	int n4, n12, n28, ntemp;

	read_room();

	if(rm4>0)									//find the starting date available for the preference of twin room
	{
		n4 = 0;
		for(a=start_i; a<=end_i-days; a++)
		{
			arms4d = 0;
			for(b=0; b<6; b++)
			{
				ad4rm = 0;
				for(c=0; c<days; c++)
					if(strcmp(room[a+max_dates*b+c].bookID,"NULL")==0)
						ad4rm = ad4rm + 1;
				if(ad4rm == days)
					arms4d = arms4d + 1;
			}
			if(arms4d >= rm4)
			{
				ok4[n4] = dates[a];
				n4++;
			}
		}				
	}
	
	if(rm12>0)									//find the starting date available for the preference of 12-people room
        {
                n12 = 0;
                for(a=start_i; a<=end_i-days; a++)
                {
                        arms4d = 0;
                        for(b=6; b<10; b++)
                        {
                                ad4rm = 0;
                                for(c=0; c<days; c++)
                                        if(strcmp(room[a+max_dates*b+c].bookID,"NULL")==0)
                                                ad4rm = ad4rm + 1;
                                if(ad4rm == days)
                                        arms4d = arms4d + 1;
                        }
                        if(arms4d >= rm12)
                        {
                                ok12[n12] = dates[a];
                                n12++;
                        }
                }
        }

	if(rm28>0)									//find the starting date available for the preference of 28-people room
        {
               	n28 = 0;
                for(a=start_i; a<=end_i-days; a++)
                {
                        arms4d = 0;
                        for(b=10; b<12; b++)
                        {
                                ad4rm = 0;
                                for(c=0; c<days; c++)
                                        if(strcmp(room[a+max_dates*b+c].bookID,"NULL")==0)
                                                ad4rm = ad4rm + 1;
                                if(ad4rm == days)
                                        arms4d = arms4d + 1;
                        }
                        if(arms4d >= rm28)
                        {
                                ok28[n28] = dates[a];
                                n28++;
                        }
                }
        }

	no_of_dates_room_ok = 0;									//set the number of dates with room available be 0;

	if(rm4>0 && rm12==0 && rm28==0)							//find the starting date available if the customer only book twin room
	{
		for(i=0; i<n4; i++)
			room_ok_start[i] = ok4[i];
		no_of_dates_room_ok = n4;
	}
	else if(rm12>0 && rm4==0 && rm28==0)						//find the starting date available if the customer only book 12-people room
	{
		for(i=0; i<n12; i++)
			room_ok_start[i] = ok12[i];
		no_of_dates_room_ok = n12;
	}
	else if(rm28>0 && rm4==0 && rm12==0)						//find the starting date available if the customer only book 28-people room
	{	
		for(i=0; i<n28; i++)
			room_ok_start[i] = ok28[i];
		no_of_dates_room_ok = n28;
	}
	else if(rm4>0 && rm12>0 && rm28==0)						//find the starting date available if the customer only book twin room and 12-people room
	{
		for(i=0; i<n4; i++)
			for(j=0; j<n12; j++)
				if(ok4[i] == ok12[j])
				{
					room_ok_start[no_of_dates_room_ok] = ok12[j];
					no_of_dates_room_ok++;
				}
	}
	else if(rm4>0 && rm28>0 && rm12==0)						//find the starting date available if the customer only book twin room and 28-people room
	{
                for(i=0; i<n4; i++)
                        for(j=0; j<n28;	j++)
                                if(ok4[i] == ok28[j])
                                {
                                        room_ok_start[no_of_dates_room_ok] = ok28[j];
                                        no_of_dates_room_ok++;
                                }
        }
	else if(rm12>0 && rm28>0 && rm4==0)						//find the starting date available if the customer only book 12-people room and 28-people room
        {
                for(i=0; i<n12; i++)
                        for(j=0; j<n28; j++)
                                if(ok12[i] == ok28[j])
                                {
                                        room_ok_start[no_of_dates_room_ok] = ok28[j];
                                        no_of_dates_room_ok++;
                                }
        }
	else if(rm4>0 && rm12>0 && rm28>0) 				//find the starting date available if the customer book twin room, 12-people room and 28-people room
	{
                ntemp = 0;
                for(i=0; i<n4; i++)
                        for(j=0; j<n12; j++)
                                if(ok4[i] == ok12[j])
                                {
                                        temp[ntemp] = ok12[j];
                                        ntemp++;
                                }
                for(i=0; i<ntemp; i++)
                        for(j=0; j<n28; j++)
			{
                                if(temp[i] == ok28[j])
                                {
                                        room_ok_start[no_of_dates_room_ok] = ok28[j];
                                        no_of_dates_room_ok++;
                                }	
			}	
	}

	if(no_of_dates_room_ok>0)
	{
		printf("\nThese are the periods with rooms available : \n");
		for(i=0; i<no_of_dates_room_ok; i++)
		{
			j=0;
			while(j<max_dates && dates[j]!=room_ok_start[i])
				j++;
			room_ok_end[i] = dates[j+days];
			room_ok_start_i[i] = j;
			printf("[%d]	%d to %d\n", i, room_ok_start[i], room_ok_end[i]);
		}
		return 1;
	}
	else
		return 0;
}

void reserve_room()
{
	char temp, str[100];
	int choice, reserved, ad4rm, rm, i, ordermeal, checkin_i, checkout_i;		//rm = which room of the same type is checking
	
	printf("\n");
	do{
		printf("Please choose the period you would like to book. (0 to %d) : ", no_of_dates_room_ok-1);
		scanf("%s%c", str, &temp);
	}while(range_check(str, 0, no_of_dates_room_ok-1)==0);
	choice = atoi(str);

	checkin_i = room_ok_start_i[choice];
	checkout_i = room_ok_start_i[choice] + days;

	sprintf(booking[total_booking].bookID, "b%d", total_booking+1);			//add a new data entry into booking[500] array 
	strcpy(booking[total_booking].cID, cID);
	booking[total_booking].checkin_date = room_ok_start[choice];
	booking[total_booking].checkout_date = room_ok_end[choice];
	booking[total_booking].ppl = ppl;
	booking[total_booking].room_cost = (rm4*400 + rm12*1200 + rm28*2800)*days;
	booking[total_booking].meal_cost = 0;
	booking[total_booking].new_cost = 0;
	strcpy(booking[total_booking].pay_status, "pending");

     	if(rm4>0)                                                                       //reserve the twin rooms for the customer
        {
		reserved=0;
		rm=0;
		while(reserved!=rm4)
		{
			ad4rm = 0;
			for(i=0; i<days; i++)
				if(strcmp(room[checkin_i+rm*max_dates+i].bookID, "NULL")==0)
					ad4rm = ad4rm + 1;
			if(ad4rm==days)
			{
				for(i=0; i<days; i++)
					strcpy(room[checkin_i+rm*max_dates+i].bookID, booking[total_booking].bookID);
				reserved += 1;
			}
			rm += 1;
		}
	}

        if(rm12>0)                                                                       //reserve the 12-people rooms for the customer
        {
                reserved=0;
                rm=6;		//r121 is the 6th room in the room array
                while(reserved!=rm12)
                {
                        ad4rm = 0;
                        for(i=0; i<days; i++)
                                if(strcmp(room[checkin_i+rm*max_dates+i].bookID, "NULL")==0)
                                        ad4rm = ad4rm + 1;
                        if(ad4rm==days)
                        {
                                for(i=0; i<days; i++)
                                        strcpy(room[checkin_i+rm*max_dates+i].bookID, booking[total_booking].bookID);
                                reserved += 1;
                        }
                        rm += 1;
                }
        }

        if(rm28>0)                                                                       //reserve the 28-people rooms for the customer
        {
                reserved=0;
                rm=10;		//r281 is the 10th room	in the room array
                while(reserved!=rm28)
                {
                        ad4rm = 0;
                        for(i=0; i<days; i++)
                                if(strcmp(room[checkin_i+rm*max_dates+i].bookID, "NULL")==0)
                                        ad4rm = ad4rm + 1;
                        if(ad4rm==days)
                        {
                                for(i=0; i<days; i++)
                                        strcpy(room[checkin_i+rm*max_dates+i].bookID, booking[total_booking].bookID);
                                reserved += 1;
                        }
                        rm += 1;
                }
        }

	printf("\nRoom reservation completed.\n");

	do{
		printf("Do you want to order group meals for your camp? (1)Yes or (2)No : ");
		scanf("%s%c", str, &temp);
	}while(range_check(str, 1, 2)==0);
	ordermeal = atoi(str);

	if(ordermeal == 1)
		order_meal(total_booking);
	else
		confirm_booking();
}

void order_meal(int booking_i) 
{
	int i, j, quantity, reserved, table, total_tables_need, tables_need, order, cost=0, checkin_i, checkout_i;
	char meal_code[2], temp;

	checkin_i=0;
	while(booking[booking_i].checkin_date!=dates[checkin_i] && checkin_i<max_dates)
		checkin_i++;
	
	checkout_i=0;
	while(booking[booking_i].checkout_date!=dates[checkout_i] && checkout_i<max_dates)
		checkout_i++;

	printf("\n		<<MEAL ORDERING (optional) >>\n\n");
	printf("There are 3 group meals to choose for each time period.\n");
	printf("(a) Chinese	$30 per person\n");
	printf("(b) Western	$35 per person\n");
	printf("(c) Vegetarian 	$25 per person\n\n");

	printf("[REMINDER] The quantity per group meal is your booking's number of participants.\n");
	printf("[REMINDER] Maximum %d people per table.\n", max_ppl_per_table);	
	printf("[REMINDER] B = Breakfast, L = Lunch, D = Dinner\n\n");

	printf("	Date		Time Period(B/L/D)	Meal Code(a/b/c/n for not ordering)\n");

	// checkin date's dinner
	
	do{													
		printf("[0] 	%d 	D		       : ", booking[booking_i].checkin_date); 
		scanf("%s", meal_code);
	}while((meal_code[0]!='a' && meal_code[0]!='b') && (meal_code[0]!='c' && meal_code[0]!='n'));
	if(meal_code[0]!='n')
	{
		if(meal_code[0] == 'a')                                            //add up the cost
                   	cost += booking[booking_i].ppl*30;
             	else if(meal_code[0] == 'b')
                   	cost += booking[booking_i].ppl*35;
            	else if(meal_code[0] == 'c')
                     	cost += booking[booking_i].ppl*25;

		reserved = 0;
		table = 0;
		quantity = booking[booking_i].ppl;
		if(booking[booking_i].ppl % max_ppl_per_table ==0)
			total_tables_need = booking[booking_i].ppl/max_ppl_per_table;
		else
			total_tables_need = booking[booking_i].ppl/max_ppl_per_table + 1;
		tables_need = total_tables_need;

		while(reserved != total_tables_need)
		{
			if(strcmp(canteen[checkin_i*max_table*3 + 2*max_table + table].bookID, "NULL") == 0)
			{
				canteen[checkin_i*max_table*3 + 2*max_table + table].meal_code[0] = meal_code[0];
				canteen[checkin_i*max_table*3 + 2*max_table + table].quantity = quantity / tables_need;
				strcpy(canteen[checkin_i*max_table*3 + 2*max_table + table].bookID, booking[booking_i].bookID);

				quantity = quantity - quantity / tables_need;
				tables_need--;
				reserved++;
			}
			table++;
		}		
	}

	//check 3 meals from the day after checkin date to the day before checkoutdate
	
	order = 1;
	for(i=checkin_i+1 ; i<checkout_i ; i++)
		for(j=0; j<3 ;j++)
		{
			do{                                    		                                                                                         
                		printf("[%d] 	%d   	%c             	       : ", order, dates[i], period[j]);
                		scanf("%s", meal_code);
        		}while((meal_code[0]!='a' && meal_code[0]!='b') && (meal_code[0]!='c' && meal_code[0]!='n'));

			if(meal_code[0] != 'n')
			{
				if(meal_code[0] == 'a')						//add up the cost
					cost += booking[booking_i].ppl*30;
				else if(meal_code[0] == 'b')
					cost += booking[booking_i].ppl*35;
				else if(meal_code[0] == 'c')
					cost += booking[booking_i].ppl*25;

				reserved = 0;
				table = 0;
				quantity = booking[booking_i].ppl;
                		if(booking[booking_i].ppl % max_ppl_per_table ==0)
                        		total_tables_need = booking[booking_i].ppl/max_ppl_per_table;
                		else
                    			total_tables_need = booking[booking_i].ppl/max_ppl_per_table + 1;
                		tables_need = total_tables_need;

                		while(reserved != total_tables_need)
				{
					if(strcmp(canteen[i*max_table*3 + j*max_table + table].bookID, "NULL") == 0)
					{
						canteen[i*max_table*3 + j*max_table + table].meal_code[0] = meal_code[0];
						canteen[i*max_table*3 + j*max_table + table].quantity = quantity / tables_need;
						strcpy(canteen[i*max_table*3 + j*max_table + table].bookID, booking[booking_i].bookID);

						quantity = quantity - quantity / tables_need;
						tables_need--;
						reserved++;
					}
					table++;	
				}
			}
			order++;
		}

       	for(j=0; j<2 ;j++)
       	{
             	do{		
                      	printf("[%d]	%d	%c		       : ", order, booking[booking_i].checkout_date, period[j]);
                      	scanf("%s", meal_code);
               	}while((meal_code[0]!='a' && meal_code[0]!='b') && (meal_code[0]!='c' && meal_code[0]!='n'));

              	if(meal_code[0] != 'n')
              	{
                   	if(meal_code[0] == 'a')                                            //add up the cost
                          	cost += booking[booking_i].ppl*30;
                       	else if(meal_code[0] == 'b')
                             	cost += booking[booking_i].ppl*35;
                     	else if(meal_code[0] == 'c')
                             	cost += booking[booking_i].ppl*25;

                      	reserved = 0;
                     	table = 0;
                       	quantity = booking[booking_i].ppl;
                	if(booking[booking_i].ppl % max_ppl_per_table ==0)
                        	total_tables_need = booking[booking_i].ppl/max_ppl_per_table;
                	else
                    		total_tables_need = booking[booking_i].ppl/max_ppl_per_table + 1;
                	tables_need = total_tables_need;

                	while(reserved != total_tables_need)
                      	{
                            	if(strcmp(canteen[checkout_i*max_table*3 + j*max_table + table].bookID, "NULL") == 0)
                              	{
                                   	canteen[checkout_i*max_table*3 + j*max_table + table].meal_code[0] = meal_code[0];
                                       	canteen[checkout_i*max_table*3 + j*max_table + table].quantity = quantity / tables_need;
                                       	strcpy(canteen[checkout_i*max_table*3 + j*max_table + table].bookID, booking[booking_i].bookID);

                                    	quantity = quantity - quantity / tables_need;
                                     	tables_need--;
                                      	reserved++;
                              	}
                            	table++;
                    	}
              	}
             	order++;
      	}

	if(amend == 0)
	{
		booking[booking_i].meal_cost = cost;
		confirm_booking();
	}
	else if(amend == 1 && complete == 0)
	{
		booking[booking_i].meal_cost = cost;
	}
	else if(amend == 1 && complete == 1)
	{
		booking[booking_i].new_cost = booking[booking_i].room_cost + cost;
	}
}

void confirm_booking()
{
	int i, confirm, choice;
	char payID[5], str[100], temp;

	show_booking_customer(total_booking);

	sprintf(payID, "p%d", total_payment+1);

	printf("\nPayment ID : %s\n\n", payID);
	printf("Please complete the payment within 1 hour, or else the booking will be cancelled.\n");
	printf("Please copy the Payment ID and proceed to the <<Complete Payment>> page for payment, thank you.\n");

	printf("\n[REMINDER] No amendments can be made on Room Booking after confirmed\n");
	printf("[REMINDER] You have to restart the booking process if you are amending Room Booking\n");

	do{
		printf("(1)Confirm, (2)Amend Room, (3)Amend Meals, (4)Cancel : ");
		scanf("%s%c", str, &temp);
	}while(range_check(str, 1, 4)==0);
	confirm = atoi(str);

	if(confirm == 1)
	{
		strcpy(payment[total_payment].payID, payID);
		strcpy(payment[total_payment].bookID, booking[total_booking].bookID);
		payment[total_payment].fee = booking[total_booking].room_cost + booking[total_booking].meal_cost;
		strcpy(payment[total_payment].method, "-");
		strcpy(payment[total_payment].status, "pending");
		total_booking += 1;
		total_payment += 1;

		do{
			printf("(1)Print or (2)Back to Main Menu : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str, 1, 2)==0);
		choice = atoi(str);		

		//if(choice==1)
			//print the booking information
	}
	else if (confirm == 2)
	{
		for(i=0; i<max_room*max_dates; i++)
			if(strcmp(room[i].bookID, booking[total_booking].bookID)==0)
				strcpy(room[i].bookID, "NULL");
		for(i=0; i<max_table*3*max_dates; i++)
			if(strcmp(canteen[i].bookID, booking[total_booking].bookID)==0)
			{
				strcpy(canteen[i].bookID, "NULL");
				canteen[i].meal_code[0] = '-';
				canteen[i].quantity = 0;
			}

		read_preference();
	}
	else if(confirm == 3)
	{
	    	for(i=0; i<max_table*3*max_dates; i++)
                        if(strcmp(canteen[i].bookID, booking[total_booking].bookID)==0)
                        {
                                strcpy(canteen[i].bookID, "NULL");
                                canteen[i].meal_code[0]	= '-';
                                canteen[i].quantity = 0;
                        }
		order_meal(total_booking);
	}
	else if(confirm == 4)
	{
                for(i=0; i<max_room*max_dates; i++)
                        if(strcmp(room[i].bookID, booking[total_booking].bookID)==0)
                                strcpy(room[i].bookID, "NULL");
                for(i=0; i<max_table*3*max_dates; i++)
                        if(strcmp(canteen[i].bookID, booking[total_booking].bookID)==0)
                        {
                                strcpy(canteen[i].bookID, "NULL");
                                canteen[i].meal_code[0] = '-';
                                canteen[i].quantity = 0;
                        }
	}
}

void show_booking_customer(int booking_i) 
{
	int i, j, k, no, rm4=0, rm12=0, rm28=0, days, price;
	int customer_i, checkin_i, checkout_i;

       	customer_i=0;
       	while(strcmp(booking[booking_i].cID, customer[customer_i].cID)!=0 && customer_i<total_customer)
               	customer_i++;

      	checkin_i=0;
     	while(booking[booking_i].checkin_date != dates[checkin_i] && checkin_i<max_dates)
             	checkin_i++;

     	checkout_i=0;
      	while(booking[booking_i].checkout_date != dates[checkout_i] && checkout_i<max_dates)
             	checkout_i++;

	printf("\n		<< BOOKING - %s >>\n\n", booking[booking_i].bookID);

	printf("Customer Information\n");
	printf("		Name : 		%s\n", customer[customer_i].name);
	printf("		D.O.B : 	%d\n", customer[customer_i].dob);
	printf("	  	Contact no. : 	%d\n", customer[customer_i].contact);
	printf("		Email : 	%s\n\n", customer[customer_i].email);

	printf("Booking Information\n");
	printf("	Booking ID : 		%s\n", booking[booking_i].bookID);
	printf("	No. of participants : 	%d\n", booking[booking_i].ppl);
	printf("	Check-in Date :		%d\n", booking[booking_i].checkin_date);
	printf("	Check-out Date :	%d\n\n", booking[booking_i].checkout_date);	

	printf("	Item			Quantity	Cost	Remarks\n\n");
	
	no=1;

	for(i=0; i<max_room; i++)
		if(strcmp(room[i*max_dates + checkin_i].bookID, booking[booking_i].bookID) == 0)
		{
			if(room[i*max_dates + checkin_i].type == 4)
				rm4++;
			else if(room[i*max_dates + checkin_i].type == 12)
				rm12++;
			else if(room[i*max_dates + checkin_i].type == 28)
				rm28++;
		}
	
	days = checkout_i - checkin_i;

	if(rm4>0)
	{
		printf("[%d]	4-people Room		%d		$%d	%d to %d\n", no, rm4, rm4*400*days, booking[booking_i].checkin_date, booking[booking_i].checkout_date);	
		no++;
	}

	if(rm12>0)
	{
                printf("[%d]	12-people Room 		%d		$%d	%d to %d\n", no, rm12, rm12*1200*days, booking[booking_i].checkin_date, booking[booking_i].checkout_date);
                no++;
	}

	if(rm28>0)
	{
		printf("[%d]	28-people Room		%d		$%d	%d to %d\n", no, rm28, rm28*2800*days, booking[booking_i].checkin_date, booking[booking_i].checkout_date);
		no++;
	}

	printf("\n");

	for(j=checkin_i*max_table*3 + max_table*2; j<= checkout_i*max_table*3 + max_table*1; j=j+max_table)
	{	
		k=0;
		while(strcmp(canteen[j+k].bookID, booking[booking_i].bookID) != 0 && k<max_table)
			k++;
		if(k<max_table)
		{
			if(canteen[j+k].meal_code[0] == 'a')
				price = 30;
			else if(canteen[j+k].meal_code[0] == 'b')
				price = 35;
			else if(canteen[j+k].meal_code[0] == 'c')
				price = 25;

			printf("[%d]	Meal %c			%d		$%d	%d ", no, canteen[j+k].meal_code[0], booking[booking_i].ppl, booking[booking_i].ppl*price, canteen[j+k].date);
			
			if(strcmp(canteen[j+k].period, "B")==0)
				printf("Breakfast\n");
			else if(strcmp(canteen[j+k].period, "L")==0)
				printf("Lunch\n");
			else if(strcmp(canteen[j+k].period, "D")==0)
				printf("Dinner\n");
			
			no++;
		}
	}
	
	if(booking[booking_i].new_cost==0)
		printf("\nTotal cost : $%d\n", booking[booking_i].room_cost + booking[booking_i].meal_cost);
	else
		printf("\nTotal cost : $%d\n", booking[booking_i].new_cost);
}

int verify_payment()
{
	//verify payments through the corresponding payment websites.
	//if the payment is completed, return 1.

	return 1;
}

void complete_payment() 
{
	int i, j, k, choice, method, paid;
	char payID[5], temp, str[100];
	
	read_payment();
	read_booking();

	printf("\n		<< COMPLETE PAYMENT >>\n\n");

	do{
		printf("Payment ID : ");
		scanf("%s%c", payID, &temp);

		i=0;
		while(strcmp(payment[i].payID, payID)!=0 && i<total_payment)
			i++;

		choice = 0;
		if(i==total_payment)
		{
			do{
				printf("No record found. (1)Try Again or (2)Back to Main Menu : ");
				scanf("%s%c", str, &temp);
			}while(range_check(str, 1, 2)==0);
			choice = atoi(str);
		}
	}while(choice==1);

	j=0;
	while(j<total_booking && strcmp(booking[j].bookID, payment[i].bookID)!=0)
		j++;

	if(strcmp(payment[i].status, "complete")==0)
		printf("\nYou have already completed your payment.\n");
	else if(strcmp(payment[i].status, "cancelled")==0)
		printf("\nThis payment has already been cancelled due to booking cancellation or amendments.\n");
	else if(i<total_payment)
	{
		printf("\nPlease choose your payment method.\n");
		do{
			printf("(1)Credit Card, (2)PayPal or (3)AliPay : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str, 1, 3)==0);		
		method = atoi(str);		

		if(method==1)
			strcpy(payment[i].method, "CreditCard");
		else if(method==2)
			strcpy(payment[i].method, "PayPal");
		else if(method==3)
			strcpy(payment[i].method, "AliPay");	
			
		paid = verify_payment();

		if(paid == 1)
		{
			strcpy(payment[i].status, "complete");
			strcpy(booking[j].pay_status, "complete");
			if(booking[j].new_cost>0)
			{
				booking[j].meal_cost = booking[j].new_cost - booking[j].room_cost;
				booking[j].new_cost = 0;
			}
		}

		printf("\nPayment completed.\n");		
	
		write_booking();
		write_payment();

		read_customer();
		read_room();
		read_canteen();

		show_booking_customer(j);

		printf("\nPayment ID : ");
		for(k=0; k<total_payment; k++)
			if(strcmp(payment[k].bookID, booking[j].bookID)==0)
				printf("%s, ", payment[k].payID);
		printf("\n");
	}
}

void amend_booking(int max_soon)
{
        int i, choice, booking_i, no=1, pending=0, pending_i, cost_difference;
        char temp, str[100];

	complete = 0;

        printf("\n              << AMEND BOOKING >>\n\n");

	printf("[REMINDER] You can only amend Meal Booking.\n");
        printf("[REMINDER] You have to pay for the difference if the new cost is higher than the original cost even after payment.\n");
	printf("[REMINDER] Refund will only be given back a day before the check-in date.\n\n");

        printf("        Bookings that you can amend : \n\n");

        for(i=0; i<max_soon; i++)
                printf("[%d]    %s              from %d to %d\n", i, soon_booking[i].bookID, soon_booking[i].checkin_date, soon_booking[i].checkout_date);
        printf("\n");

        do{
           	printf("Please choose the booking you would like to amend (from 0 to %d) : ", max_soon-1);
                scanf("%s%c", str, &temp);
        }while(range_check(str, 0, max_soon-1)==0);
	choice = atoi(str);

        booking_i = 0;
        while(strcmp(booking[booking_i].bookID, soon_booking[choice].bookID)!=0 && booking_i<total_booking)
                booking_i++;

        show_booking_customer(booking_i);

        printf("\nPayment status : %s\n\n", booking[booking_i].pay_status);
        for(i=0; i<total_payment; i++)
                if(strcmp(payment[i].bookID, booking[booking_i].bookID)==0)
                {
                        printf("[%d]    %s	$%d     %s\n", no, payment[i].payID, payment[i].fee, payment[i].status);
                        if(strcmp(payment[i].status, "complete")==0)
                                complete = 1;
			else if(strcmp(payment[i].status, "pending")==0)
			{
				pending = 1;
				pending_i = i;
			}
			no++;
                }

	printf("\n");
        do{
           	printf("Are you sure that you would like to amend your booking? (1)Yes or (2)No : ");
                scanf("%s%c", str, &temp);
        }while(range_check(str, 1, 2)==0);
	choice = atoi(str);

        if(choice == 1)
	{
		for(i=0; i<max_table*3*max_dates; i++)
                        if(strcmp(canteen[i].bookID, booking[booking_i].bookID)==0)
                        {
                                strcpy(canteen[i].bookID, "NULL");
                                canteen[i].meal_code[0] = '-';
                                canteen[i].quantity = 0;
                        }

		order_meal(booking_i);
		show_booking_customer(booking_i);	// bkmk
		
		cost_difference = booking[booking_i].new_cost - booking[booking_i].room_cost - booking[booking_i].meal_cost;

		if(complete==0 && pending==1)
			payment[pending_i].fee = booking[booking_i].room_cost + booking[booking_i].meal_cost;
		else if(cost_difference < 0 && booking[booking_i].new_cost!=0)
		{
			if(complete==1 && pending==1)
			{
				strcpy(payment[pending_i].status, "cancelled");
				strcpy(booking[booking_i].pay_status, "complete");
			}
		}
		else if(cost_difference > 0)
		{
			if(complete==1 && pending==1)
				payment[pending_i].fee = cost_difference;
			else if(complete==1 && pending==0)
			{
				sprintf(payment[total_payment].payID, "p%d", total_payment+1);
				strcpy(payment[total_payment].bookID, booking[booking_i].bookID);
				payment[total_payment].fee = cost_difference;
                		strcpy(payment[total_payment].method, "-");
                		strcpy(payment[total_payment].status, "pending");
				strcpy(booking[booking_i].pay_status, "pending");
                		total_payment += 1;
			}
		}

       		printf("\nPayment status : %s\n\n", booking[booking_i].pay_status);
		no = 1;
        	for(i=0; i<total_payment; i++)
                	if(strcmp(payment[i].bookID, booking[booking_i].bookID)==0)
			{
                        	printf("[%d]	%s	$%d	%s\n", no, payment[i].payID, payment[i].fee, payment[i].status);
				no++;
			}
		printf("\nPlease complete the pending payment as soon as possible if there is any.\n\n");
		do{
			printf("(1)Print or (2)Back to Main Menu : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str, 1, 2)==0);
		choice = atoi(str);
		//if(choice==1)
			//print the booking information
	}
}

void cancel_booking(int max_soon) 
{
	int i, choice, booking_i, no=1, complete=0;
	char temp, str[100];
	
	printf("\n		<< CANCEL BOOKING >>\n\n");

	printf("[REMINDER] You can only receive 75%% refund if you cancel your booking after completing your payment.\n\n");

	printf("	Bookings that you can cancel : \n\n");

	for(i=0; i<max_soon; i++)
		printf("[%d]	%s		from %d to %d\n", i, soon_booking[i].bookID, soon_booking[i].checkin_date, soon_booking[i].checkout_date);
	printf("\n");

	do{
		printf("Please choose the booking you would like to cancel (from 0 to %d) : ", max_soon-1);
		scanf("%s%c", str, &temp);
	}while(range_check(str, 0, max_soon-1)==0);
	choice = atoi(str);

	booking_i = 0;
	while(strcmp(booking[booking_i].bookID, soon_booking[choice].bookID)!=0 && booking_i<total_booking)
		booking_i++;

	show_booking_customer(booking_i);
	
	printf("\nPayment status : %s\n\n", booking[booking_i].pay_status);
	for(i=0; i<total_payment; i++)
		if(strcmp(payment[i].bookID, booking[booking_i].bookID)==0)
		{
			printf("[%d]	%s	$%d	%s\n", no, payment[i].payID, payment[i].fee, payment[i].status);
			if(strcmp(payment[i].status, "complete")==0)
				complete = 1;
			no++;
		}

	printf("\n");
	do{
		printf("Are you sure that you would like to cancel your booking? (1)Yes or (2)No : ");
		scanf("%s%c", str, &temp);
	}while(range_check(str, 1, 2)==0);
	choice = atoi(str);

	if(choice == 1)
	{
               	for(i=0; i<max_dates*max_room; i++)
                   	if(strcmp(room[i].bookID, booking[booking_i].bookID)==0)
                             	strcpy(room[i].bookID, "NULL");
              	for(i=0; i<max_dates*max_table*3; i++)
                     	if(strcmp(canteen[i].bookID, booking[booking_i].bookID)==0)
                      	{
                            	strcpy(canteen[i].bookID, "NULL");
                              	canteen[i].meal_code[0] = '-';
                              	canteen[i].quantity = 0;
                       	}

		if(complete == 1)
		{
			printf("75%% refund would be transferred back to you through your payment account.\n");
			for(i=0; i<total_payment; i++)
				if(strcmp(payment[i].bookID, booking[booking_i].bookID)==0 && strcmp(payment[i].status, "pending")==0)
					strcpy(payment[i].status, "cancelled");
			booking[booking_i].new_cost = (booking[booking_i].room_cost + booking[booking_i].meal_cost) * 0.25;
			strcpy(booking[booking_i].pay_status, "cancelled");	
			printf("Your booking is cancelled. The transaction will be done within the next 3 days.\n");
		}
		else
		{
			for(i=0; i<total_payment; i++)
				if(strcmp(payment[i].bookID, booking[booking_i].bookID)==0)
					strcpy(payment[i].status, "cancelled");
                        strcpy(booking[booking_i].pay_status, "cancelled");
                        printf("Your booking is	cancelled.\n");
		}
	}
}

void staff_log_in()
{
	FILE *read;
	char staff[100][30], pw[100][30];
	char id[30], pw1[30], temp, str[100];
	int i=0, total_staff, staff_choice, matched, error_choice;
	struct termios term, term_orig;
        tcgetattr(STDIN_FILENO, &term);
        term_orig = term;
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);

	read = fopen("staff.txt", "r");

	fscanf(read, "%s", staff[i]);
	fscanf(read, "%s", pw[i]);

	while(!feof(read))
	{
		i++;
		fscanf(read, "%s", staff[i]);
		fscanf(read, "%s", pw[i]);
	}

	total_staff = i;
	
	fclose(read);

	printf("\n		<<STAFF ONLY>>\n");
	
        do{
                printf("\nStaff username : ");
		scanf("%s%c", id, &temp);

                printf("Password : ");
                tcsetattr(STDIN_FILENO, TCSANOW, &term);        //disabling the input echo function to hide the password
                fgets(pw1, 30, stdin);
                pw1[strlen(pw1)-1] = '\0';

                tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);   //enabling the input echo function

                i=0;
                while(!(strcmp(id,staff[i])==0 && strcmp(pw1, pw[i])==0) && i<total_staff)
                        i++;

                if(i==total_staff)
                {
                        matched = 0;
                        printf("\n\nRecord not found. (1)Try again or (2)Back to Main Menu?\n");
                        do{
                           	printf("Please choose your action : ");
                                scanf("%s%c", str, &temp);
                        }while(range_check(str, 1, 2)==0);
			error_choice = atoi(str);
                }
                else if(i<total_staff)
                {
                        matched = 1;
                }
        }while(matched == 0 && error_choice == 1);

        if(matched == 1)
        {
		printf("\n");
		do{
			printf("Please choose one allocation to check. (1)Booking, (2)Room or (3)Meal : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str, 1, 3)==0);
		staff_choice = atoi(str);

		if(staff_choice==1)
			show_booking_staff();
		else if(staff_choice==2)
			show_room();
		else if(staff_choice==3)
			show_canteen();
        }
}

void show_booking_staff() 
{
       	int i, j, k, no, choice, refund, next;
        int customer_i, checkin_i, checkout_i, booking_i;
	char bookID[5], str[100], temp;

	read_room();
	read_canteen();
	read_booking();
	read_payment();
	read_customer();
	
	do{
		do{
			printf("Booking ID : ");
			scanf("%s", bookID);

			booking_i = 0;
			while(booking_i<total_booking && strcmp(booking[booking_i].bookID, bookID)!=0)
				booking_i++;

			choice=0;
			if(booking_i == total_booking)
			{
				do{
					printf("No record found. (1)Try Again or (2)Back to Main Menu : ");
					scanf("%s%c", str, &temp);
				}while(range_check(str, 1, 2)==0);
				choice = atoi(str);
			}
		}while(choice==1);

		next = 0;
		if(choice==0)
		{
        		customer_i=0;
        		while(strcmp(booking[booking_i].cID, customer[customer_i].cID)!=0 && customer_i<total_customer)
                		customer_i++;

        		checkin_i=0;
        		while(booking[booking_i].checkin_date != dates[checkin_i] && checkin_i<max_dates)
                		checkin_i++;

        		checkout_i=0;
        		while(booking[booking_i].checkout_date != dates[checkout_i] && checkout_i<max_dates)
                		checkout_i++;
	
        		printf("\n		<< BOOKING - %s >>\n\n", booking[booking_i].bookID);

        		printf("Customer Information\n");
        		printf("    		Name :   	%s\n", customer[customer_i].name);
        		printf("    		D.O.B :   	%d\n", customer[customer_i].dob);
        		printf("  		Contact no. :  	%d\n", customer[customer_i].contact);
        		printf("    		Email :    	%s\n\n", customer[customer_i].email);

        		printf("Booking Information\n");
        		printf("	Booking ID :      	%s\n", booking[booking_i].bookID);
        		printf("	No. of participants : 	%d\n", booking[booking_i].ppl);
        		printf("	Check-in Date :  	%d\n", booking[booking_i].checkin_date);
        		printf("	Check-out Date : 	%d\n\n", booking[booking_i].checkout_date);
	
			if(strcmp(booking[booking_i].pay_status, "cancelled") !=0)
       			{
		 		printf("	Item			Quantity	Remarks			ID\n\n");

				no=1;

				for(i=0; i<max_room*max_dates; i++)
					if(strcmp(room[i].bookID, bookID)==0)
					{
						printf("[%d]	%d-people Room				%d		%s\n", no, room[i].type, room[i].date, room[i].roomID);
						no++;
					}
				printf("\n");
	
				for(i=0; i<max_dates*max_table*3; i++)
					if(strcmp(canteen[i].bookID, bookID)==0)
					{
						printf("[%d]	Meal %c			%d		%d ", no, canteen[i].meal_code[0], canteen[i].quantity, canteen[i].date);
						if(canteen[i].period[0] == 'B')
							printf("Breakfast	");
						else if(canteen[i].period[0] == 'L')
							printf("Lunch		");
						else if(canteen[i].period[0] == 'D')
							printf("Dinner		");
						printf("%s\n", canteen[i].tableID);
						no++;
					}
			}
			printf("\nPayment status : %s\n\n", booking[booking_i].pay_status);

			for(i=0; i<total_payment; i++)
				if(strcmp(payment[i].bookID, bookID)==0)
					printf("%s	$%d	%s\n", payment[i].payID, payment[i].fee, payment[i].status);

			refund = booking[booking_i].room_cost + booking[booking_i].meal_cost - booking[booking_i].new_cost;
			
			printf("\n");
			if(refund>0 && booking[booking_i].new_cost!=0)		
				printf("Refund = $%d\n\n", refund);

			do{
				printf("(1)Check Other Booking, (2)Check Room Allocations, (3)Check Canteen Allocations, (4)Back to Main Menu or (5)Print: ");
				scanf("%s%c", str, &temp);
			}while(range_check(str, 1, 5)==0);
			next = atoi(str);
		}
	}while(next==1);

	if(next==2)
		show_room();
	else if(next==3)
		show_canteen();
//	else if(next==5)
		// print as receipt for giving custoner
}

void show_room()
{
        int i, j, choice;
	char temp, str[100];

	read_room();

	printf("\n		<< ROOM ALLOCATION (%d to %d) >>\n\n", dates[0], dates[max_dates-1]);

	printf("\t");
	for(i=0; i<max_room; i++)
		printf("\t%s", room_id[i]);
	printf("\n\n");

	for(i=0; i<max_dates; i++)
	{
		printf("%d",dates[i]);
		for(j=0; j<max_room; j++)
			printf("\t%s", room[j*max_dates + i].bookID);
		printf("\n");		 
	}

	printf("\n");
	do{
		printf("(1)Check Booking, (2)Check Canteen Allocations or (3)Back to Main Menu : ");
		scanf("%s%c", str, &temp);
	}while(range_check(str, 1, 3)==0);
	choice = atoi(str);	

	if(choice == 1)
		show_booking_staff();
	else if(choice == 2)
		show_canteen();
}
	
void show_canteen()
{
	int i, date, date_i, within, period, a, b, c, choice;
	char temp, str[100];

	read_canteen();

	do{
		printf("\n");
		do{
			do{
				printf("Date from %d to %d (YYYYMMDD) : ", dates[0], dates[max_dates-1]);
				scanf("%s%c", str, &temp);
			}while(range_check(str, dates[0], dates[max_dates-1])==0);
			date = atoi(str);

			date_i=0;
			while(date!=dates[date_i] && date_i<max_dates)
				date_i++;

			within = 1;
			if(date_i == max_dates)
				within = 0;
		}while(within == 0);

		do{
			printf("(0)Breakfast, (1)Lunch or (2)Dinner? : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str, 0, 2)==0);
		period = atoi(str);

		printf("\n	Booking ID	A	B	C\n\n");

		a=0;
		b=0;
		c=0;

		for(i=0; i<max_table; i++)
		{
			printf("%s \t%s \t\t", table_id[i], canteen[date_i*max_table*3 + period*max_table + i].bookID);

			if(canteen[date_i*max_table*3 + period*max_table + i].meal_code[0] == 'a')
			{
				printf("%d\n",canteen[date_i*max_table*3 + period*max_table + i].quantity);
				a += canteen[date_i*max_table*3 + period*max_table + i].quantity;
			}
			else if(canteen[date_i*max_table*3 + period*max_table + i].meal_code[0] == 'b')
			{
				printf("\t%d\n", canteen[date_i*max_table*3 + period*max_table + i].quantity);
				b += canteen[date_i*max_table*3 + period*max_table + i].quantity;
			}
			else if(canteen[date_i*max_table*3 + period*max_table + i].meal_code[0] == 'c')
			{
				printf("\t\t%d\n", canteen[date_i*max_table*3 + period*max_table + i].quantity);
				c += canteen[date_i*max_table*3 + period*max_table + i].quantity;
			}
			else
				printf("\n");
		}
		printf("\t\tTotal :\t%d \t%d \t%d \n\n", a, b, c);

		do{
			printf("(1)Check other dates and periods, (2)Check Booking, (3)Check Room Allocations or (4)Back to Main Menu : ");
			scanf("%s%c", str, &temp);
		}while(range_check(str, 1, 4)==0);
		choice = atoi(str);
	}while(choice == 1);

	if(choice == 2)
		show_booking_staff();
	else if(choice == 3)
		show_room();
}

int main()
{
	int choice;
	char str[100], temp;

	set_today();
	date_included();
	update_room_dates();
	update_canteen_dates();
	logged_in=0;

	do{
		checked_room_availability = 0;	// reset to 0 everytime coz not yet check availability
		
		printf("\nWelcome to our campsite's Online Booking Website!\n\n");
        	printf("Please select one action: \n");
        	printf("[1] Enquiry for Room Availability\n");
        	printf("[2] Booking (Amend/Cancel/New booking)\n");
        	printf("[3] Complete your Payment\n");
        	printf("[4] Display Allocations (Staff Only)\n");
        	printf("[5] Exit\n\n");

        	do{
           		printf("Please select one action : ");
                	scanf("%s%c", str, &temp);
        	}while(range_check(str, 1, 5)==0);
		choice = atoi(str);

		if(choice == 1)
		{
			printf("\n		<<ENQUIRY FOR ROOM AVAILABILITY>>\n");
			read_preference();
		}
		else if(choice == 2)
			booking_();
		else if(choice==3)
			complete_payment();
		else if(choice==4)
			staff_log_in();
		else if(choice==5)
			exit(0);
	}while(choice != 5);
	
	return 0;
}
