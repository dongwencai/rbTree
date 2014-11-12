#ifndef __RBTREELIST_H__
#define __RBTREELIST_H__

#include "pthread.h"

typedef unsigned int rbhdl_t;

#define ARG_END    ((void *)0xffffffff)

typedef enum{
    RED,
    BLACK,
}RB_COLOR;

typedef enum{
    RB_SUC,
    RB_FAIL,
    RB_NODE_NOT_EXIST,
    RB_TREE_NOT_EXIST,
}RB_STATUS;

typedef struct rb_node {
    RB_COLOR color;
    struct rb_node *father;
    struct rb_node *lChild;
    struct rb_node *rChild;
    void *date;
}RB_NODE,*PRB_NODE;

typedef struct{
    PRB_NODE root;
    pthread_mutex_t lock;
}RB_INFO,*PRB_INFO;
extern RB_STATUS insert_rb_node_ex(PRB_INFO pRBInfo,void *p);

extern RB_STATUS erase_rb_node_ex(PRB_INFO pRBInfo,void *p);

extern RB_STATUS insert_rb_node(PRB_INFO pRBInfo, ...);

extern void print_tree_by_mid_order(PRB_INFO pRBInfo); 

extern int rb_init(PRB_INFO pRBInfo);

extern RB_STATUS erase_rb_all_node(PRB_INFO pRBInfo);
#endif
