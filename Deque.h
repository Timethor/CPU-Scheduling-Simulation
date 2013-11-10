/*
 * File:   Deque.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef DEQUE_H
#define	DEQUE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Contains the deque, deque nodes and iterator
#define DEFINE_DEQUE(type) \
    typedef struct type##_dequeN { \
        struct type##_dequeN* next; \
        struct type##_dequeN* prev; \
        type* data; \
    } type##_dequeN ; \
    typedef struct type##_deque { \
        type##_dequeN* head; \
        type##_dequeN* tail; \
        bool debug; \
        bool trace; \
    } type##_deque; \
    typedef struct type##_dequeI { \
        type##_dequeN* current; \
        type##_deque* container; \
    } type##_dequeI;

// Prototypes deque functions
#define DEQUE_PROTOTYPE(type) \
    DEFINE_DEQUE(type) \
    \
    void type##_deque_init(type##_deque* container, bool debug, bool trace); \
    void type##_dequeN_init(type##_dequeN* container); \
    \
    void type##_deque_pushB(type##_deque* container, type##_dequeN* node, type* data); \
    void type##_deque_pushF(type##_deque* container, type* data); \
    void type##_deque_pushA(type##_deque* container, type##_dequeN* node, type* data); \
    void type##_deque_pushL(type##_deque* container, type* data); \
    \
    type* type##_deque_pollF(type##_deque* container); \
    type* type##_deque_pollL(type##_deque* container); \
    type* type##_deque_pollN(type##_deque* container, type* data); \
    void type##_deque_freeElements(type##_deque* container); \
    \
    type* type##_deque_peekF(type##_deque* container); \
    type* type##_deque_peekL(type##_deque* container); \
    type##_dequeN* type##_deque_getCar(type##_deque* container, type* data); \
    \
    bool  type##_deque_empty(type##_deque* container); \
    int   type##_deque_length(type##_deque* container); \
    \
    void type##_dequeI_init(type##_dequeI* it, type##_deque* container); \
    type* type##_dequeI_next(type##_dequeI* it); \
    type* type##_dequeI_examine(type##_dequeI* it); \
    type* type##_dequeI_remove(type##_dequeI* it); \
 

// Defines type specific insertion functions ()
#define DEQUE_INSERTION(type) \
    void type##_deque_first_insert(type##_deque* container, type* data){ \
        type##_dequeN* newnode = malloc(sizeof(*newnode)); \
        newnode->data= data; \
        newnode->next=NULL; \
        newnode->prev=NULL; \
        container->head = newnode; \
        container->tail = newnode; \
    } \
    \
    void type##_deque_pushF(type##_deque * container, type* data) { \
        if (data == NULL){ \
        } \
        if (container->head==NULL) { \
            type##_deque_first_insert(container, data); \
        } else { \
            type##_deque_pushB(container, container->head, data); \
        } \
    } \
    \
    void type##_deque_pushA(type##_deque * container, type##_dequeN* node, type* data) { \
        type##_dequeN* newnode = malloc(sizeof(*newnode)); \
        if (container->tail->data == node->data) { \
            container->tail = newnode; \
        } \
        newnode->data = data; \
        type##_dequeN* nextnode = node->next; \
        newnode->next = nextnode; \
        if (nextnode != NULL) nextnode->prev = newnode; \
        newnode->prev = node; \
        node->next = newnode; \
    } \
    \
    void type##_deque_pushL(type##_deque * container, type* data) { \
        if (data == NULL){ \
        } \
        if (container->tail==NULL) { \
            type##_deque_first_insert(container, data); \
        } else { \
            type##_deque_pushA(container, container->tail, data); \
        } \
    } \
    \
    void type##_deque_pushB(type##_deque * container, type##_dequeN* node, type* data) { \
        type##_dequeN* newnode = malloc(sizeof(*newnode)); \
        if (container->head->data == node->data) { \
            container->head = newnode; \
        } \
        newnode->data = data; \
        type##_dequeN* prevnode = node->prev; \
        newnode->prev = prevnode; \
        if (prevnode != NULL) prevnode->next = newnode; \
        newnode->next = node; \
        node->prev = newnode; \
    }

// Defines type specific removal functions (pollN, pollF, pollL)
#define DEQUE_REMOVAL(type) \
    type* type##_deque_pollN(type##_deque* container, type* data){ \
        if (container->head==NULL) { \
            return NULL; \
        } \
        if (type##_deque_peekF(container) == data) { \
            return type##_deque_pollF(container); \
        } \
        if (type##_deque_peekL(container) == data){ \
            return type##_deque_pollL(container); \
        } \
        type##_dequeN* first = container->head->next; \
        type##_dequeN* previous = NULL; \
        while (first != NULL) { \
            if (first->data == data) { \
                type##_dequeN* ret = first; \
                type* data = ret->data; \
                previous = first->prev; \
                first = first->next; \
                previous->next = first; \
                first->prev = previous; \
                if (container->head!=NULL) container->head->prev = NULL; \
                else container->tail=NULL; \
                free(ret); \
                return data; \
            } \
            first = first->next; \
        } \
        return NULL; \
    } \
    \
    type* type##_deque_pollF(type##_deque* container){ \
        if (container->head==NULL){ \
            return NULL; \
        } \
        type##_dequeN* first = container->head; \
        container->head=first->next; \
        type* data = first->data; \
        if (container->head!=NULL) container->head->prev = NULL; \
        else container->tail=NULL; \
        free(first); \
        return data; \
    } \
    \
    type* type##_deque_pollL(type##_deque* container){ \
        if (container->tail==NULL) { \
            return NULL; \
        } \
        type##_dequeN* last = container->tail; \
        container->tail=last->prev; \
        type* data = last->data; \
        if (container->tail==NULL) container->head=NULL; \
        else container->tail->next = NULL; \
        free(last); \
        return data; \
    } \
    \
    void type##_deque_freeElements(type##_deque* container){ \
        while (!type##_deque_empty(container)) { \
            type* data = type##_deque_pollF(container); \
            type##_destruct(data); \
        } \
    }

// Defines type specific status function
#define DEQUE_STATUS(type) \
    bool type##_deque_empty(type##_deque* container){ \
        if (container->head==NULL) { \
            return true; \
        } \
        return false; \
    } \
    \
    int type##_deque_length(type##_deque* container){ \
        type##_dequeN* current = container->head; \
        int length=0; \
        while (current != NULL) { \
            current=current->next; \
            length++; \
        } \
        return length; \
    } \
    \
    type* type##_deque_peekF(type##_deque* container){ \
        if (container->head==NULL) return NULL; \
        return container->head->data; \
    } \
    \
    type* type##_deque_peekL(type##_deque* container){ \
        if (container->tail==NULL) return NULL; \
        return container->tail->data; \
    } \
    \
    type##_dequeN* type##_deque_getCar(type##_deque* container, type* data){ \
        if (container->head==NULL) { \
            return NULL; \
        } \
        if (type##_deque_peekF(container) == data) { \
            return container->head; \
        } \
        if (type##_deque_peekL(container) == data){ \
            return container->tail; \
        } \
        type##_dequeN* first = container->head->next; \
        while (first != NULL) { \
            if (first->data == data) { \
                return first; \
            } \
            first = first->next; \
        } \
        return NULL; \
    } \
    \

// Defines type specific iterator function
#define DEQUE_ITERATOR(type) \
    void type##_dequeI_init(type##_dequeI* it, type##_deque* container) { \
        it->current = container->head; \
        it->container = container; \
    } \
    \
    type* type##_dequeI_next(type##_dequeI* it) { \
        if (it->current == NULL) return NULL; \
        it->current = it->current->next; \
        return type##_dequeI_examine(it); \
    } \
    \
    type* type##_dequeI_examine(type##_dequeI* it) { \
        if (it->current != NULL) \
            return it->current->data; \
        return NULL; \
    } \
    \
    type* type##_dequeI_remove(type##_dequeI* it) { \
        if (it->current==NULL) return NULL; \
        if (it->current->prev==NULL) \
            return type##_deque_pollF(it->container); \
        type##_dequeN* temp = it->current; \
        temp->prev->next = temp->next; \
        temp->next->prev = temp->prev; \
        it->current = temp->next; \
        type* data = temp->data; \
        free(temp); \
        return data; \
    }

// Defines type specific removal functions (pollF, pollL)
#define DEQUE_INIT(type) \
    void type##_deque_init(type##_deque* container, bool debug, bool trace) { \
        container->head = NULL; \
        container->tail = NULL; \
        container->debug = debug; \
        container->trace = trace; \
    } \
    \
    void type##_dequeN_init(type##_dequeN* container) { \
        container->next = NULL; \
        container->prev = NULL; \
        container->data = NULL; \
    }


// Runs all type specific function macros
#define DEQUE(type) \
	DEQUE_INSERTION(type) \
	DEQUE_REMOVAL(type) \
	DEQUE_STATUS(type) \
	DEQUE_ITERATOR(type) \
    DEQUE_INIT(type) \


#endif	/* DEQUE_H */

