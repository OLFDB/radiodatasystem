/**
 * \file rdstmc.c
 * \brief Dumps received RDS TMC data.
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This program dumps all received TMC data from a RDS stream.
 */

#include <stdio.h>
#include "traff.h"
#include "string.h"
#include <malloc/_malloc.h>
#include <semaphore.h>

Feed feed;


//<feed>
//<message id="tmc:5.1.1:5.1.1327.n.1" receive_time="2017-02-15T21:01:28+01:00"
//update_time="2017-02-15T21:07:00+01:00" expiration_time="2017-02-
//15T21:22:00+01:00">
//<events>
//<event class="CONGESTION" type="CONGESTION_SLOW_TRAFFIC"/>
//</events>
//<location road_class="MOTORWAY" road_ref="A4" fuzziness="LOW_RES">
//<from junction_name="Trezzo">+45.59612 +9.50253</from>
//<to junction_name="Dalmine">+45.64412 +9.62081</to>
//</location>
//</message>
//</feed>

static void get_traff_time(time_t _t, char* buf)
{
    struct tm *tm;

    tm = localtime(&_t);
    strftime(buf, 40, "%FT%T%z", tm);
}

sem_t semaphore;
bool seminit = false;

static void print_traff_feed_to_file(FILE *file) {
    printf("<feed>\n");
    for(int i=0; i<feed.count;i++) {
        Message *msg=feed.Message;
        if(msg[i].IdAttr==0) {
            printf("No. %i -- Required element is null: message.id\n", i);
            continue;
        }
        printf("\t<message id=\"%s\" ", msg[i].IdAttr);
        if(msg[i].Receive_timeAttr) {
            printf("receive_time=%s ", msg[i].Receive_timeAttr);
        }
        if(msg[i].Update_timeAttr) {
            printf("update_time=%s ", msg[i].Update_timeAttr);
        }
        if(msg[i].Expiration_timeAttr) {
            printf("expiration_time=%s ", msg[i].Expiration_timeAttr);
        }
        printf(">\n");
        printf("\t</message>\n");        
    }
    printf("</feed>\n");
}

void traff_create_from_tmc_message(rds_oda_tmc_message_t *msg) {
    
    if(!seminit) {
        sem_init(&semaphore, 0, 1);
        seminit=true;
    }
    
    Message Message1;
    memset(&Message1, 0, sizeof(Message1));

    
    char traff_msg[1024];
    char rcvt[40];
    get_traff_time(msg->receive_time, rcvt);
    char updt[40];
    get_traff_time(msg->start_time, updt);
    char expt[40];
    get_traff_time(msg->expiry_time, expt);
    
    // Prepare Traff message
    // Allocate enough memory
    int size = snprintf(NULL, 0, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc) + 1;
    Message1.IdAttr = malloc(size);
   
    // store id
    snprintf(Message1.IdAttr, size, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc);
    
    if(((char *)strstr(Message1.IdAttr, "tmc:0.0.0:0.0"))!=NULL)
        return;
    
    Message* msg_to_upd = 0;
    find_traff_message(msg, &msg_to_upd);
    
    // receive_time
    if(msg->receive_time!=0) {
        size = snprintf(NULL,0,"%s", rcvt)+1;
        Message1.Receive_timeAttr = malloc(size);
        snprintf(Message1.Receive_timeAttr, size, "%s", rcvt);
    } else {
        printf("Element empty: Message.Receive_timeAttr\n");
    }
    
    // update_time
    if(msg->start_time!=0) {
        size = snprintf(NULL,0,"%s", updt)+1;
        Message1.Update_timeAttr = malloc(size);
        snprintf(Message1.Update_timeAttr, size, "%s", updt);
    } else {
        printf("Element empty: Message.Update_timeAttr\n");
    }
    
    // expiration_time
    if(msg->expiry_time!=0) {
        size = snprintf(NULL,0,"%s", expt)+1;
        Message1.Expiration_timeAttr = malloc(size);
        snprintf(Message1.Expiration_timeAttr, size, "%s", expt);
    } else {
        printf("Element empty: Message.Expiration_timeAttr\n");
    }
    
    sem_wait(&semaphore);
    if(msg_to_upd) {
        // replace old message with updated one
        memcpy(msg_to_upd, &Message1, sizeof(Message));
    } else {
        
        // finally add the message to the global feed
        // allocate new memory
        void* address = malloc(sizeof(Message)*(feed.count+1));
        
        if(feed.count > 0) {
            // We already had messages in the feed, copy them to the new message array and free the memory of old message array
            memcpy(address, feed.Message, sizeof(Message)*(feed.count));
            free(feed.Message);
        }
        
        // Set the new message array
        feed.Message = address;
        // copy the new message to the end of the array and increase the message count of the feed
        void* address2 = address+sizeof(Message)*(feed.count);
        memcpy(address2, &Message1, sizeof(Message));
        // increase count of messages
        feed.count++;
    }
    sem_post(&semaphore);
    //print_traff_feed_to_file(NULL);
    
    // print single message
    
//    strncpy(traff_msg, "<feed>\n", sizeof("<feed>\n"));
//    strcat(traff_msg, "\t<message id=\"");
//    snprintf(&traff_msg[21], 1003, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i\" receive_time=\"%s\" update_time=\"%s\" expiration_time=\"%s\">\n", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc, rcvt, updt, expt);
//    
//    snprintf(&traff_msg[strlen(traff_msg)], 1024 - strlen(traff_msg)-1, "\t\t<events>\n\t\t\t<event class=\"CONGESTION\" type=\"CONGESTION_SLOW_TRAFFIC\"/>\n\t\t</events>\n\t\t<location roadclass=\"%s\" roadref=\"%s\" fuzziness=\"%s\">\n\t\t\t<from junction_name=\"%s\">%s</from>\n\t\t\t<to junction_name=\"%s\">%s</to>\n\t\t</location>","","","","","","","");
//    
//    strcat(traff_msg, "\n\t</message>\n</feed>\n");
//    printf("%s",traff_msg);
}

void remove_traff_message(rds_oda_tmc_message_t *msg) {
    Message Message1;
    int size = snprintf(NULL, 0, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc) + 1;
    Message1.IdAttr = malloc(size);
    snprintf(Message1.IdAttr, size, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc);
    
    if(((char *)strstr(Message1.IdAttr, "tmc:0.0.0:0.0"))!=NULL)
        return;
    
    sem_wait(&semaphore);
    /* find message in list */
    for(int i=0; i<feed.count; i++) {
        Message* address2 = feed.Message+sizeof(Message)*(i);
        Message* msgtorem = address2;
        if(!strcmp(address2->IdAttr, Message1.IdAttr)) {
            if(i>(feed.count-1)) {
                // move last message to this position
                address2=&address2[feed.count-1-i];
            }
            feed.count--;
            break;
        }
    }
    
    Message* newmsgs = malloc(feed.count * sizeof(Message));
    memcpy(newmsgs, feed.Message, feed.count*sizeof(Message));
    free(feed.Message);
    feed.Message=newmsgs;
    sem_post(&semaphore);
    
}

void find_traff_message(rds_oda_tmc_message_t *msg, Message** traff_msg) {
    Message Message1;
    int size = snprintf(NULL, 0, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc) + 1;
    Message1.IdAttr = malloc(size);
    snprintf(Message1.IdAttr, size, "tmc:%x.%x.%x:%x.%i.%i.c.%i,%i", msg->cc, msg->ltn, msg->sid,msg->cc, msg->ltn, msg->loc, msg->dir, msg->upc);
    sem_wait(&semaphore);
    /* find message in list */
    for(int i=0; i<feed.count; i++) {
        Message* address2 = feed.Message+sizeof(Message)*(i);
        Message* msgtorem = address2;
        if(!strcmp(address2->IdAttr, Message1.IdAttr)) {
            *traff_msg=address2;
            break;
        }
    }
    sem_post(&semaphore);
}
