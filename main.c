#include "stdio.h"
#include "unistd.h"
#include "rbTreeList.h"
#include <pthread.h>
#include <stdlib.h>

int date1 = 10,date2 = 20,date3 = 30,date4 = 40,date5 = 50,date6 = 60,date7=35,date8 = 32,date9 = 31;

int main (int argc, char const* argv[])
{
    PRB_NODE root=NULL;
    PRB_INFO rb_handle = NULL,rb_handle1 = NULL,rb_handle2 = NULL;
    rb_handle = (PRB_INFO)malloc(sizeof(RB_INFO));
    rb_handle1 = (PRB_INFO)malloc(sizeof(RB_INFO));
    rb_handle2 = (PRB_INFO)malloc(sizeof(RB_INFO));
    
    rb_init(rb_handle);
    //rb_init(rb_handle1);
    //rb_init(rb_handle2);
    insert_rb_node(rb_handle,&date3,&date2,&date6,&date4,&date5,&date7,&date8,&date9,&date1,ARG_END);
    erase_rb_all_node(rb_handle);
    sleep(1);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    erase_rb_node_ex(rb_handle,&date7);
    erase_rb_node_ex(rb_handle,&date9);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    erase_rb_node_ex(rb_handle,&date6);
    erase_rb_node_ex(rb_handle,&date5);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    erase_rb_node_ex(rb_handle,&date4);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    
    erase_rb_node_ex(rb_handle,&date1);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    erase_rb_node_ex(rb_handle,&date2);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    erase_rb_node_ex(rb_handle,&date3);
    print_tree_by_mid_order(rb_handle);
    printf("\n");
    erase_rb_node_ex(rb_handle,&date8);
    /*insert_rb_node_ex(rb_handle1,&date3);
    insert_rb_node_ex(rb_handle1,&date2);
    insert_rb_node_ex(rb_handle2,&date1);
    insert_rb_node_ex(rb_handle2,&date4);
    */
    return 0;   

}
