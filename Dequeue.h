/* 
 * File:   Dequeue.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef DEQUEUE_H
#define	DEQUEUE_H

#include <stdbool.h>
#include <stdio.h>

// Contains the dequeue, dequeue nodes and iterator
#define DEFINE_DEQUEUE(type) \
    typedef struct type##_dequeueN { \
        struct type##_dequeueN* next; \
        struct type##_dequeueN* prev; \
        type* data; \
    } type##_dequeueN ; \
    typedef struct type##_dequeue { \
        type##_dequeueN* head; \
        type##_dequeueN* tail; \
    } type##_dequeue; \
    typedef struct type##_dequeueI { \
        type##_dequeueN* current; \
        type##_dequeue* container; \
    } type##_dequeueI;

// Prototypes dequeue functions
#define DEQUEUE_PROTOTYPE(type) \
    DEFINE_DEQUEUE(type) \
    \
    void type##_dequeue_init(type##_dequeue* container); \
    void type##_dequeueN_init(type##_dequeueN* container); \
    \
    void type##_dequeue_pushF(type##_dequeue* container, type* data); \
    void type##_dequeue_pushA(type##_dequeueN* node, type* data); \
    void type##_dequeue_pushB(type##_dequeueN* node, type* data); \
    void type##_dequeue_pushL(type##_dequeue* container, type* data); \
    \
    type* type##_dequeue_pollF(type##_dequeue* container); \
    type* type##_dequeue_pollL(type##_dequeue* container); \
    \
    type* type##_dequeue_peekF(type##_dequeue* container); \
    type* type##_dequeue_peekL(type##_dequeue* container); \
    \
    bool  type##_dequeue_empty(type##_dequeue* container); \
    int   type##_dequeue_length(type##_dequeue* container); \
    \
    void type##_dequeueI_init(type##_dequeueI* it, type##_dequeue* container); \
    type* type##_dequeueI_next(type##_dequeueI* it); \
    type* type##_dequeueI_examine(type##_dequeueI* it); \
    type* type##_dequeueI_remove(type##_dequeueI* it); \
 

// Defines type specific insertion functions ()
#define DEQUEUE_INSERTION(type) \
    void type##_dequeue_first_insert(type##_dequeue* container, type* data){ \
        type##_dequeueN* newnode = malloc(sizeof(*newnode)); \
        newnode->data= data; \
        newnode->next=NULL; \
        container->head = newnode; \
        container->tail = newnode; \
    } \
    \
    void type##_dequeue_pushF(type##_dequeue * container, type* data) { \
        if (container->head==NULL) \
            type##_dequeue_first_insert(container, data); \
        else { \
            type##_dequeue_pushA(container->head, data); \
            container->head=container->head->prev; \
        } \
    } \
    \
    void type##_dequeue_pushA(type##_dequeueN* node, type* data) { \
        type##_dequeueN* newnode = malloc(sizeof(*newnode)); \
        newnode->data = data; \
        type##_dequeueN* nextnode = node->next; \
        newnode->next = nextnode; \
        node->next = newnode; \
    } \
    \
    void type##_dequeue_pushL(type##_dequeue * container, type* data) { \
        if (container->tail==NULL)\
            type##_dequeue_first_insert(container, data); \
        else { \
            type##_dequeue_pushB(container->tail, data); \
            container->tail=container->tail->next; \
        } \
    } \
    \
    void type##_dequeue_pushB(type##_dequeueN* node, type* data) { \
        type##_dequeueN* newnode = malloc(sizeof(*newnode)); \
        newnode->data = data; \
        newnode->next = node; \
        type##_dequeueN* prevnode = node->prev; \
        if (prevnode != NULL) prevnode->next = newnode; \
        node->next = newnode; \
    }

// Defines type specific removal functions (pollF, pollL)
#define DEQUEUE_REMOVAL(type) \
    type* type##_dequeue_pollF(type##_dequeue* container){ \
        if (container->head==NULL) return NULL; \
        type##_dequeueN* first = container->head; \
        container->head=first->next; \
        type* data = first->data; \
        free(first); \
        if (container->head==NULL) container->tail=NULL; \
        return data; \
    } \
    \
    type* type##_dequeue_pollL(type##_dequeue* container){ \
        if (container->tail==NULL) return NULL; \
        type##_dequeueN* last = container->tail; \
        container->tail=last->prev; \
        type* data = last->data; \
        free(last); \
        if (container->tail==NULL) container->head=NULL; \
        return data; \
    }

// Defines type specific status function
#define DEQUEUE_STATUS(type) \
    bool type##_dequeue_empty(type##_dequeue* container){ \
        if (container->head==NULL) \
            return true; \
        return false; \
    } \
    \
    int type##_dequeue_length(type##_dequeue* container){ \
        type##_dequeueN* current = container->head; \
        int length=0; \
        while (current != NULL) { \
            current=current->next; \
            length++; \
        } \
        return length; \
    } \
    \
    type* type##_dequeue_peekF(type##_dequeue* container){ \
        if (container->head==NULL) return NULL; \
        return container->head->data; \
    } \
    \
    type* type##_dequeue_peekL(type##_dequeue* container){ \
        if (container->tail==NULL) return NULL; \
        return container->tail->data; \
    }

// Defines type specific iterator function
#define DEQUEUE_ITERATOR(type) \
    void type##_dequeueI_init(type##_dequeueI* it, type##_dequeue* container) { \
        it->current = container->head; \
        it->container = container; \
    } \
    \
    type* type##_dequeueI_next(type##_dequeueI* it) { \
        if (it->current == NULL) return NULL; \
        it->current = it->current->next; \
        return type##_dequeueI_examine(it); \
    } \
    \
    type* type##_dequeueI_examine(type##_dequeueI* it) { \
        if (it->current != NULL) \
            return it->current->data; \
        return NULL; \
    } \
    \
    type* type##_dequeueI_remove(type##_dequeueI* it) { \
        if (it->current==NULL) return NULL; \
        if (it->current->prev==NULL) \
            return type##_dequeue_pollF(it->container); \
        type##_dequeueN* temp = it->current; \
        temp->prev->next = temp->next; \
        it->current = temp->next; \
        type* data = temp->data; \
        free(temp); \
        return data; \
    }

// Defines type specific removal functions (pollF, pollL)
#define DEQUEUE_INIT(type) \
    void type##_dequeue_init(type##_dequeue* container){ \
        container->head = NULL; \
        container->tail = NULL; \
    } \
    \
    void type##_dequeueN_init(type##_dequeueN* container){ \
        container->next = NULL; \
        container->prev = NULL; \
        container->data = NULL; \
    }
    

// Runs all type specific function macros
#define DEQUEUE(type) \
	DEQUEUE_INSERTION(type) \
	DEQUEUE_REMOVAL(type) \
	DEQUEUE_STATUS(type) \
	DEQUEUE_ITERATOR(type) \
    DEQUEUE_INIT(type) \


#endif	/* DEQUEUE_H */

