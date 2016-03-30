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

static inline int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_destroy(pthread_attr_t *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getdetachstate(const pthread_attr_t *, int *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getguardsize(const pthread_attr_t *, size_t *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getinheritsched(const pthread_attr_t *, int *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getschedparam(const pthread_attr_t *, struct sched_param *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getschedpolicy(const pthread_attr_t *, int *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getscope(const pthread_attr_t *, int *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getstack(const pthread_attr_t *, void **, size_t *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getstackaddr(const pthread_attr_t *, void **) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_getstacksize(const pthread_attr_t *, size_t *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_init(pthread_attr_t *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setdetachstate(pthread_attr_t *, int) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setguardsize(pthread_attr_t *, size_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setinheritsched(pthread_attr_t *, int) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setschedparam(pthread_attr_t *,
							   const struct sched_param *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setschedpolicy(pthread_attr_t *, int) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setscope(pthread_attr_t *, int) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setstack(pthread_attr_t *, void *, size_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setstackaddr(pthread_attr_t *, void *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_attr_setstacksize(pthread_attr_t *, size_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_cancel(pthread_t) __DARWIN_ALIAS(pthread_cancel) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_detach(pthread_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_equal(pthread_t, pthread_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline void pthread_exit(void *) __dead2 {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_getconcurrency(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_getschedparam(pthread_t , int *,
						  struct sched_param *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline void* pthread_getspecific(pthread_key_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_join(pthread_t , void **) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_key_create(pthread_key_t *, void (*)(void *)) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_key_delete(pthread_key_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline pthread_t pthread_self(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_setcancelstate(int , int *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_setcanceltype(int , int *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_setconcurrency(int) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_setschedparam(pthread_t, int, const struct sched_param *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_setspecific(pthread_key_t , const void *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline void pthread_testcancel(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

/* returns non-zero if pthread_create or cthread_fork have been called */
static inline int pthread_is_threaded_np(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_threadid_np(pthread_t,__uint64_t*) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

/*SPI to set and get pthread name*/
static inline int pthread_getname_np(pthread_t,char*,size_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_setname_np(const char*) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

/* returns non-zero if the current thread is the main thread */
static inline int pthread_main_np(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline size_t pthread_get_stacksize_np(pthread_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline void* pthread_get_stackaddr_np(pthread_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

/* Like pthread_create(), but leaves the thread suspended */
static inline int pthread_create_suspended_np(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_kill(pthread_t, int) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline pthread_t pthread_from_mach_thread_np(mach_port_t) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline int pthread_sigmask(int, const sigset_t *, sigset_t *) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}

static inline void pthread_yield_np(void) {
	kprintf(""__func__" not implemented.\n");
	return 0;
}
