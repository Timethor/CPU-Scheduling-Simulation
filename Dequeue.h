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

// Contains the dequeueue, dequeue nodes and iterator
#define DEFINE_DEQUEUE(type) \
    typedef struct dequeueN_##type { \
        struct dequeueN_##type* next; \
        struct dequeueN_##type* prev; \
        type* data; \
    } dequeueN_##type ; \
    typedef struct dequeue_##type { \
        dequeueN_##type* head; \
        dequeueN_##type* tail; \
    } dequeue_##type; \
    typedef struct dequeueI_##type { \
        dequeueN_##type* current; \
        dequeue_##type* container; \
    } dequeueI_##type;

// Prototypes dequeue functions
#define DEQUEUE_PROTOTYPE(type) \
    DEFINE_DEQUEUE(type) \
    \
    void dequeue_##type##_pushF(dequeue_##type* container, type* data); \
    void dequeue_##type##_pushA(dequeueN_##type* node, type* data); \
    void dequeue_##type##_pushB(dequeueN_##type* node, type* data); \
    void dequeue_##type##_pushL(dequeue_##type* container, type* data); \
    \
    type* dequeue_##type##_pollF(dequeue_##type* container); \
    type* dequeue_##type##_pollL(dequeue_##type* container); \
    \
    type* dequeue_##type##_peekF(dequeue_##type* container); \
    type* dequeue_##type##_peekA(dequeueN_##type* node, type* data); \
    type* dequeue_##type##_peekL(dequeue_##type* container); \
    \
    bool  dequeue_##type##_empty(dequeue_##type* container); \
    int   dequeue_##type##_length(dequeue_##type* container); \
    \
    void dequeueI_##type##_init(dequeueI_##type* it, dequeue_##type* container); \
    type* dequeueI_##type##_next(dequeueI_##type* it); \
    type* dequeueI_##type##_examine(dequeueI_##type* it); \
    type* dequeueI_##type##_remove(dequeueI_##type* it); \
 

// Defines type specific insertion functions ()
#define DEQUEUE_INSERTION(type) \
    void dequeue_##type##_first_insert(dequeue_##type* container, type* data){ \
        dequeueN_##type* newnode = malloc(sizeof(*newnode)); \
        newnode->data= data; \
        newnode->next=NULL; \
        container->head = newnode; \
        container->tail = newnode; \
    } \
    \
    void dequeue_##type##_pushF(dequeue_##type * container, type* data) { \
        if (container->head==NULL)\
            dequeue_##type##_first_insert(container, data); \
        else { \
            dequeue_##type##_insert_after(container->head, data); \
            container->head=container->head->prev; \
        } \
    } \
    \
    void dequeue_##type##_pushA(dequeueN_##type* node, type* data) { \
        dequeueN_##type* newnode = malloc(sizeof(*newnode)); \
        newnode->data = data; \
        dequeueN_##type* nextnode = node->next; \
        newnode->next = nextnode; \
        node->next = newnode; \
    } \
    \
    void dequeue_##type##_pushL(dequeue_##type * container, type* data) { \
        if (container->tail==NULL)\
            dequeue_##type##_first_insert(container, data); \
        else { \
            dequeue_##type##_insert_before(container->tail, data); \
            container->tail=container->tail->next; \
        } \
    } \
    \
    void dequeue_##type##_pushB(dequeueN_##type* node, type* data) { \
        dequeueN_##type* newnode = malloc(sizeof(*newnode)); \
        newnode->data = data; \
        newnode->next = node; \
        dequeueN_##type* prevnode = node->prev; \
        if (prevnode != null) prevnode->next = newnode; \
        node->next = newnode; \
    }

// Defines type specific removal functions (pollF, pollL)
#define DEQUEUE_REMOVAL(type) \
    type* dequeue_##type##_pollF(dequeue_##type* container){ \
        if (container->head==NULL) return NULL; \
        dequeueN_##type* first = container->head; \
        container->head=first->next; \
        type* data = first->data; \
        free(first); \
        if (container->head==NULL) container->tail=NULL; \
        return data;\
    } \
    \
    type* dequeue_##type##_pollL(dequeue_##type* container){ \
        if (container->tail==NULL) return NULL; \
        dequeueN_##type* last = container->tail; \
        container->tail=last->prev; \
        type* data = last->data; \
        free(last); \
        if (container->tail==NULL) container->head=NULL; \
        return data; \
    }

// Defines type specific status function
#define DEQUEUE_STATUS(type) \
    bool dequeue_##type##_empty(dequeue_##type* container){ \
        if (container->head==NULL) \
            return true; \
        return false; \
    } \
    \
    int dequeue_##type##_length(dequeue_##type* container){ \
        dequeueN_##type* current = container->head; \
        int length=0; \
        while (current != NULL) { \
            current=current->next; \
            length++; \
        } \
        return length; \
    } \
    \
    int dequeue_##type##_peekF(dequeue_##type* container){ \
        if (container->head==NULL) return null; \
        return container->head->data; \
    } \
    \
    int dequeue_##type##_peekL(dequeue_##type* container){ \
        if (container->tail==NULL) return null; \
        return container->tail->data; \
    }

// Defines type specific iterator function
#define DEQUEUE_ITERATOR(type) \
    void dequeueI_##type##_init(dequeueI_##type* it, dequeue_##type* container) { \
        it->current = container->head; \
        it->container = container; \
    } \
    \
    type* dequeueI_##type##_next(dequeueI_##type* it) {\
        if (it->current == NULL) return NULL; \
        it->current = it->current->next; \
        return dequeue_##type##_examine(it); \
    } \
    \
    type* dequeueI_##type##_examine(dequeueI_##type* it) {\
        if (it->current != NULL) \
            return it->current->data; \
        return NULL; \
    } \
    \
    type* dequeueI_##type##_remove(dequeueI_##type* it) {\
        if (it->current==NULL) return NULL; \
        if (it->current->prev==NULL) \
            return dequeue_##type##_pollF(it->container); \
        dequeueN_##type* temp = it->current; \
        temp->prev->next = temp->next; \
        it->current = temp->next; \
        type* data = temp->data; \
        free(temp); \
        return data; \
    }\
	
// Runs all type specific function macros
#define DEQUEUE(type) \
	DEQUEUE_INSERTION(type) \
	DEQUEUE_REMOVAL(type) \
	DEQUEUE_STATUS(type) \
	DEQUEUE_ITERATOR(type) \


#endif	/* DEQUEUE_H */

