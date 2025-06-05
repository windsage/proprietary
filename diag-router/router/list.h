/*
 * Copyright (c) 2016-2018, Linaro Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>

#ifndef container_of
#define container_of(ptr, type, member) ({ \
		const typeof(((type *)0)->member)*__mptr = (ptr);  \
		(type *)((char *)__mptr - offsetof(type, member)); \
		})
#endif

struct list_head {
	pthread_mutex_t mutex;
	struct list_head *prev;
	struct list_head *next;
};

#define LIST_INIT(list) {PTHREAD_MUTEX_INITIALIZER, &(list), &(list)}

#define LIST_LOCK(list) pthread_mutex_lock(&((list)->mutex))

#define LIST_UNLOCK(list) \
	pthread_mutex_unlock(&((list)->mutex))

static inline void list_init(struct list_head *list)
{
	pthread_mutex_init(&((list)->mutex), NULL);
	list->prev = list->next = list;
}

static inline void list_destroy(struct list_head *list)
{
	pthread_mutex_destroy(&((list)->mutex));
	list->prev = list->next = NULL;
}

static inline bool list_empty(struct list_head *list)
{
	if (!list)
		return false;

	return list->next == list;
}

static inline void list_add(struct list_head *list, struct list_head *item)
{
	struct list_head *prev = NULL;

	if(!item)
		return;

	LIST_LOCK(list);

	prev = list->prev;
	item->next = list;
	item->prev = prev;
	prev->next = list->prev = item;

	LIST_UNLOCK(list);
}

static inline void list_push(struct list_head *list, struct list_head *item)
{
	struct list_head *next;

	if(!item)
		return;

	LIST_LOCK(list);

	next = list->next;
	item->prev = list;
	item->next = next;

	next->prev = list->next = item;

	LIST_UNLOCK(list);

}

static inline void list_del(struct list_head *item)
{
	if(!item || !item->prev || !item->next)
		return;

	LIST_LOCK(item);

	item->prev->next = item->next;
	item->next->prev = item->prev;

	LIST_UNLOCK(item);

}

#define list_for_each(item, list) \
	for (item = (list)->next; item != list; item = item->next)

/*NOTE:must call LIST_UNLOCK when returning/after this macro */
#define list_for_each_with_lock(item, list) \
	LIST_LOCK(list);\
        for (item = (list)->next; item != list; item = item->next)

#define list_for_each_safe(item, n, list) \
	for (item = (list)->next, n = item->next; item != (list); item = n, n = item->next)

/*NOTE:must call LIST_UNLOCK when returning/after this macro */
#define list_for_each_safe_with_lock(item, n, list) \
	LIST_LOCK(list);\
	for (item = (list)->next, n = item->next; item != (list); item = n, n = item->next)


#define list_entry(item, type, member) \
	container_of(item, type, member)

#define list_entry_first(list, type, member) \
	container_of((list)->next, type, member)

#define list_entry_next(item, member) \
	container_of((item)->member.next, typeof(*(item)), member)

#define list_for_each_entry(item, list, member) \
	for (item = list_entry_first(list, typeof(*(item)), member); \
	     &item->member != list; \
	     item = list_entry_next(item, member))

#define list_for_each_entry_with_lock(item, list, member) \
	LIST_LOCK(list);\
	for (item = list_entry_first(list, typeof(*(item)), member); \
		&item->member != list; \
		item = list_entry_next(item, member))


#define list_for_each_entry_safe(item, next, list, member) \
	for (item = list_entry_first(list, typeof(*(item)), member), \
	     next = list_entry_next(item, member); \
	     &item->member != list; \
	     item = next, \
	     next = list_entry_next(item, member)) \

#define list_for_each_entry_safe_with_lock(item, next, list, member) \
	LIST_LOCK(list);\
	for (item = list_entry_first(list, typeof(*(item)), member), \
		next = list_entry_next(item, member); \
		&item->member != list; \
		item = next, \
		next = list_entry_next(item, member)) \


static inline int list_count(struct list_head *list)
{
	struct list_head *item;
	int i = 0;

	list_for_each(item, list)
		i++;

	return i;
}

#endif
