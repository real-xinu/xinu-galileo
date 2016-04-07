/* The functions defined below will serve as a pthread-like interface to Xinu's pre-existing concurrent process scheme.
 * Moving forward with development, the plan is to start with the bare basics and add more functions as time goes on.
 *
 * Author: aryker
 **/

#define PTHREAD_CREATE_JOINABLE      1
#define PTHREAD_CREATE_DETACHED      2

#define PTHREAD_INHERIT_SCHED        1
#define PTHREAD_EXPLICIT_SCHED       2

#define PTHREAD_CANCEL_ENABLE        0x01  /* Cancel takes place at next cancellation point */
#define PTHREAD_CANCEL_DISABLE       0x00  /* Cancel postponed */
#define PTHREAD_CANCEL_DEFERRED      0x02  /* Cancel waits until cancellation point */
#define PTHREAD_CANCEL_ASYNCHRONOUS  0x00  /* Cancel occurs immediately */

/* Value returned from pthread_join() when a thread is canceled */
#define PTHREAD_CANCELED	     ((void *) 1)

/* We only support PTHREAD_SCOPE_SYSTEM */
#define PTHREAD_SCOPE_SYSTEM         1
#define PTHREAD_SCOPE_PROCESS        2

#define PTHREAD_PROCESS_SHARED         1
#define PTHREAD_PROCESS_PRIVATE        2


/* Structure for a pthread wrapper around Xinu semaphores */
struct pthread_attr_t {
  //TODO
};


/* Free the given pthread_attr_t structure, and cancel any associated processes. */
static inline int pthread_attr_destroy(pthread_attr_t *) {  
	kprintf(""__func__" not implemented.\n");
	return 0;
}


/* Initialize a pthread_attr_t structure for future use. */
static inline int pthread_attr_init(pthread_attr_t *) { //TODO: Add an argument to this that can represent many flags ANDed together to initialize a meaningful structure(the details of this will depend on the contents of the structure).
	kprintf(""__func__" not implemented.\n");
	return 0;
}


/* Send a cancellation request to the process associated with the given pthread_attr_t structure */
static inline int pthread_cancel(pthread_attr_t) __DARWIN_ALIAS(pthread_cancel) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}


/*Create a new pthread using the pthread_attr_t that is passed.*/
static inline int pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}


/* Terminate the calling pthread. */
static inline void pthread_exit(void *) __dead2 {
	kprintf(""__func__" not implemented.\n");
	return 0;
}


/* Wait for the given thread to terminate, then return. */
static inline int pthread_join(pthread_attr_t , void **) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}


/* returns non-zero if the current thread is the main thread */
static inline int pthread_main_np(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}



/* Like pthread_create(), but leaves the thread suspended 
 *
 * TODO: This may not work as written below, but needs to be included in some form.
 **/

/*
static inline int pthread_create_suspended_np(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}
*/

/* Send a signal to the given thread */
static inline int pthread_kill(pthread_t_attr_t, int) { //Personal note: This function is terribly named. Why did they do this?
	kprintf(""__func__" not implemented.\n");
	return 0;
}


