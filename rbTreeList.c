/*=============================================================================
#     FileName: rbTreeList.c
#         Desc:  
#       Author: dongwencai
#        Email: mrdong0826@gmail.com
#     HomePage: git@github.com/dongwencai
#      Version: 0.0.1
#   LastChange: 2014-11-06 13:50:59
#      History:
=============================================================================*/
#include <stdarg.h>
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "pthread.h"
#include "rbTreeList.h"
#include "common.h"
#define DEBUG
/*
#ifdef DEBUG
    #define DEBUG(fmt,arg...)  printf(fmt,##arg)
#else
    #define DEBUG(fmt,arg...)
#endif
*/

static const RB_NODE nil = {.color = BLACK,};

#define NIL (&nil)

int rb_init(PRB_INFO pRBInfo);

static PRB_NODE get_rb_node_gfather(const PRB_NODE node);

static PRB_NODE get_rb_node_uncle(const PRB_NODE node);

static PRB_INFO get_rb_tree_root_info(PRB_INFO rb_handle);

RB_STATUS insert_rb_node_ex(PRB_INFO pRBInfo,void *p);

RB_STATUS erase_rb_node_ex(PRB_INFO pRBInfo,void *p);

static int free_rb_node(PRB_NODE node);

static void *delete_all_nodes_thread(void *p);

RB_STATUS erase_rb_all_node(PRB_INFO pRBInfo);

static int free_rb_node(PRB_NODE node);

static PRB_NODE  delete_one_node(PRB_NODE node,PRB_NODE root);

RB_STATUS insert_rb_node(PRB_INFO pRBInfo, ...);

static PRB_NODE search_insert_node_by_key(PRB_NODE t,int key);

static PRB_NODE search_node_by_key(PRB_NODE t,int key);

static PRB_NODE rb_insert_rebalance(PRB_NODE node,PRB_NODE root);

static PRB_NODE rb_erase_rebalance(PRB_NODE child,PRB_NODE father,PRB_NODE root);

static PRB_NODE rb_rebalance(PRB_NODE node,PRB_NODE root);

static PRB_NODE right_rotate(PRB_NODE node,PRB_NODE root);

static PRB_NODE left_rotate(PRB_NODE node,PRB_NODE root);

static void mid_order_ex(PRB_NODE root);

void print_tree_by_mid_order(PRB_INFO pRBInfo);

int rb_init(PRB_INFO pRBInfo)
{
    pRBInfo->root = NIL;
    pthread_mutex_init(&pRBInfo->lock,NULL);
    return 0;
}
static PRB_NODE get_rb_node_gfather(const PRB_NODE node)
{
    if(node && node->father)
        return node->father->father;
    return NULL;
}

static PRB_NODE get_rb_node_uncle(const PRB_NODE node)
{
    PRB_NODE gfather = NULL;
    if(gfather = get_rb_node_gfather(node))
    {
        if(gfather->lChild == node->father)
            return gfather->rChild;
        else
            return gfather->lChild;
    }
    else
    {
        return NULL;
    }
}

static PRB_NODE get_prev_node_mid(PRB_NODE node)
{
    PRB_NODE left = node->lChild;
    while(left->lChild != NIL)
    {
        if(left->rChild != NIL)
        {
            left = left->rChild;
            break;
        }
        left = left->lChild;
    }
    return left;
}

RB_STATUS insert_rb_node_ex(PRB_INFO pRBInfo,void *p)
{
    int nKey = *(int *)p;
    PRB_NODE root = NULL,node_father = NULL,new_node = NULL;
    if(!pRBInfo)
    {
        return RB_FAIL;
    }
    pthread_mutex_lock(&pRBInfo->lock);
    root = pRBInfo->root;
    if(root != NIL)
    {
        int fk;
        node_father = search_insert_node_by_key(root,nKey);
        fk = *(int *)node_father->date;
        new_node = (PRB_NODE)malloc(sizeof(RB_NODE));
        new_node->color = RED;
        new_node->father = node_father;
        new_node->date = p;
        new_node->lChild = new_node->rChild = NIL;
        if(nKey > fk)
        {
            node_father->rChild = new_node;
        }
        else
        {
            node_father->lChild = new_node;
        }
    }
    else
    {
        new_node = (PRB_NODE)malloc(sizeof(RB_NODE));
        new_node->color = BLACK;
        new_node->father = NULL;
        new_node->date = p;
        new_node->lChild = new_node->rChild = NIL;
        root = new_node;
    }
    root = rb_insert_rebalance(new_node,root);
    pRBInfo->root = root;
    pthread_mutex_unlock(&pRBInfo->lock);
    return RB_SUC;
}

RB_STATUS erase_rb_node_ex(PRB_INFO pRBInfo,void *p)
{
    
    PRB_NODE root = NULL,node = NULL,child = NULL,father = NULL;
    int pk = *(int *)p;
    int color;
    pthread_mutex_lock(&pRBInfo->lock);
    root = pRBInfo->root;
    node = search_node_by_key(root,pk);
    if(node == NIL)
    {
        pthread_mutex_unlock(&pRBInfo->lock);
        return RB_NODE_NOT_EXIST;
    }
    if(node->rChild != NIL && node->lChild != NIL)
    {
        PRB_NODE prev = NULL;
        prev = get_prev_node_mid(node);
        SWAP(node->date,prev->date);
        node = prev;
    }
    color = node->color;
    father = node->father;
    child = (node->lChild != NIL) ? node->lChild : node->rChild;
    if(father)
    {
        if(father->lChild == node)
        {
            father->lChild = child;
        }
        else
        {
            father->rChild = child;
        }
    }
    else
    {
        root = child;
    }
    free(node);
    if(child != NIL)
    {
        child->color = BLACK;
        child->father = father;
        goto ok;
    }
    if(color == BLACK)
    {
        root = rb_erase_rebalance(child,father,root);        //因为child可能是NIL  因此child->father可能是NULL 
    }
ok:
    pRBInfo->root = root;
    pthread_mutex_unlock(&pRBInfo->lock);
    return RB_SUC;
}
static int free_rb_node(PRB_NODE node)
{
    if(node)
    {
        free(node);
    }
    return 0;
}
static PRB_NODE  delete_one_node(PRB_NODE node,PRB_NODE root)
{
    PRB_NODE child = NULL,father = NULL;
    if(node->rChild != NIL && node->lChild != NIL)
        node = get_prev_node_mid(node);
    father = node->father;
    child = (node->lChild != NIL) ? node->lChild : node->rChild;
    if(child != NIL)
    {
        child->father = father;
    }
    if(father)
    {
        if(father->lChild == node)
        {
            father->lChild = child;
        }
        else
        {
            father->rChild = child;
        }
    }
    else
    {
        root = child;
    }
    free_rb_node(node);
    return root;
}
static void *delete_all_nodes_thread(void *p)
{
    PRB_NODE root = (PRB_NODE)p;
    while(root != NIL)
    {
        root = delete_one_node(root,root);
    }
}
RB_STATUS erase_rb_all_node(PRB_INFO pRBInfo)
{
    pthread_t del_threadHdl;
    pthread_attr_t thread_attr;
    PRB_NODE root = NULL;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
    pthread_mutex_lock(&pRBInfo->lock);
    root = pRBInfo->root;
    pthread_create(&del_threadHdl,&thread_attr,delete_all_nodes_thread,(void *)root);
    pRBInfo->root = NIL;
    pthread_mutex_unlock(&pRBInfo->lock);
    pthread_attr_destroy(&thread_attr);
    return RB_SUC;
}
RB_STATUS insert_rb_node(PRB_INFO pRBInfo, ...)
{
    va_list va;
    void *p = NULL;
    va_start(va,pRBInfo);
    while((p = va_arg(va,void *)) != ARG_END)
    {
        insert_rb_node_ex(pRBInfo,p);
    }
    va_end(va);
}

static PRB_NODE search_insert_node_by_key(PRB_NODE t,int key)
{
    int pk;
    while(t !=NIL)
    {
        pk = *(int *)t->date;
        if(key > pk)
        {
            if (t->rChild == NIL) break;
            t = t->rChild;
        }
        else
        {
            if (t->lChild == NIL) break;
            t = t->lChild;
        }
    }
    return t;
}

static PRB_NODE search_node_by_key(PRB_NODE t,int key)
{
    int pk;
    while(t !=NIL)
    {
        pk = *(int *)t->date;
        if(pk == key)
        {
            break;
        }
        t = key > pk ? t->rChild : t->lChild;
    }
    return t;
}

static PRB_NODE rb_insert_rebalance(PRB_NODE node,PRB_NODE root)
{
    PRB_NODE father = NULL,uncle = NULL,gfather = NULL;
    if((father = node->father) && (father->color == RED))
    {
        uncle = get_rb_node_uncle(node);
        gfather = get_rb_node_gfather(node);
        if(uncle->color == RED)
        {
            father->color = BLACK;
            uncle->color = BLACK;
            if(gfather->father)
            {
                gfather->color = RED;
                root = rb_insert_rebalance(gfather,root);
            }
            else
            {
                gfather->color = BLACK;
                root = gfather;
            }
        }
        else
        {
            if(gfather->lChild == father)
            {
                if(father->rChild == node)
                {
                    root = left_rotate(father,root);
                    SWAP(node,father);
                }
                root = right_rotate(gfather,root);
            }
            else
            {
                if(father->lChild == node)
                {
                    root = right_rotate(father,root);
                    SWAP(node,father);
                }
                root = left_rotate(gfather,root);
            }

            father->color = BLACK;
            gfather->color = RED;
        }
    }
    return root;
}

static PRB_NODE rb_erase_rebalance(PRB_NODE node,PRB_NODE father,PRB_NODE root)
{
    PRB_NODE s = NULL;
    if(father)
    {
        s = (father->lChild == node) ? father->rChild : father->lChild;
        if(s->color == RED)
        {
            father->color = RED;
            s->color = BLACK;
            if(father->lChild == node)
            {
                root = left_rotate(father,root);
            }
            else
            {
                root = right_rotate(father,root);
            }
            root = rb_erase_rebalance(node,father,root);
        }
        else
        {
            if((s->lChild->color == BLACK) && (s->rChild->color == BLACK) && (father->color == BLACK))
            {
                s->color = RED;
                root = rb_erase_rebalance(father,father->father,root);
            }
            else if((s->lChild->color == BLACK) && (s->rChild->color == BLACK) && (father->color == RED))
            {
               SWAP(s->color,father->color);
            }
            else
            {
                if(father->lChild == node)
                {
                    if((s->lChild->color == RED) && (s->rChild->color == BLACK))
                    {
                        SWAP(s->lChild->color,s->color);
                        root = right_rotate(s,root);
                    }
                    s->color = father->color;
                    father->color = BLACK;
                    if(s->rChild->color != BLACK)   //可能是NIL节点！
                    {
                        s->rChild->color = BLACK;
                    }
                    root = left_rotate(father,root);
                }
                else
                {
                    if((s->rChild->color == RED) && (s->lChild->color == BLACK))
                    {
                        SWAP(s->rChild->color,s->color);
                        root = left_rotate(s,root);
                    }
                    s->color = father->color;
                    father->color = BLACK;
                    if(s->lChild->color != BLACK)   //可能是NIL节点！
                    {
                        s->lChild->color = BLACK;
                    }
                    root = right_rotate(father,root);
                }
            }
        }
    }
    else
    {
        root = node;
    }
    return root;
}

static PRB_NODE right_rotate(PRB_NODE node,PRB_NODE root)
{
    PRB_NODE left = node->lChild;
    if((node->lChild = left->rChild) != NIL)
    {
        left->rChild->father = node;
    }
    left->rChild = node;  
    if(left->father = node->father)
    {
        if(node->father->lChild == node)
        {
            node->father->lChild = left;
        }
        else
        {
            node->father->rChild = left;
        }
    }
    else
    {
        left->color = BLACK;
        root = left;
    }
    node->father = left;
    return root;
}

static PRB_NODE left_rotate(PRB_NODE node,PRB_NODE root)
{
    PRB_NODE right = node->rChild;
    if((node->rChild = right->lChild) != NIL)
    {
        right->lChild->father = node;
    }
    right->lChild = node;
    if(right->father = node->father)
    {
        if(node->father->lChild == node)
        {
            node->father->lChild = right;
        }
        else
        {
            node->father->rChild = right;
        }
    }
    else
    {
        right->color = BLACK;
        root = right;
    }
    node->father = right;
    return root;
}

static void mid_order_ex(PRB_NODE root)
{
    if(root != NIL)
    {
        mid_order_ex(root->lChild);
        printf("%d\t",*(int *)root->date);
        mid_order_ex(root->rChild);
    }
}

void print_tree_by_mid_order(PRB_INFO pRBInfo)
{
    PRB_NODE root = NULL;
    if(!pRBInfo)    return ;
    pthread_mutex_lock(&pRBInfo->lock);
    root = pRBInfo->root;
    if(root != NIL)
    {
        mid_order_ex(root);
    }
    pthread_mutex_unlock(&pRBInfo->lock);
    return;
}


