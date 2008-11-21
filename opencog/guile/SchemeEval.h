/*
 * SchemeEval.h
 *
 * Simple scheme expression evaluator
 * Copyright (c) 2008 Linas Vepstas <linas@linas.org>
 */

#ifndef OPENCOG_SCHEME_EVAL_H
#define OPENCOG_SCHEME_EVAL_H
#ifdef HAVE_GUILE

#include <string>
#include <pthread.h>
#include <libguile.h>

namespace opencog {

class SchemeSmob;

class SchemeEval
{
	private:
		// Initialization stuff
		void init(void);
		static void * c_wrap_init(void *);
		void per_thread_init(void);

		// Things related to evaluation
		std::string do_eval(const std::string &);
		static void * c_wrap_eval(void *);
		const std::string *pexpr;
		std::string answer;

		std::string input_line;
		bool pending_input;

		// Error handling stuff
		SCM error_string_port;
		SCM captured_stack;
		static SCM preunwind_handler_wrapper(void *, SCM, SCM);
		static SCM catch_handler_wrapper(void *, SCM, SCM);
		SCM preunwind_handler(SCM, SCM);
		SCM catch_handler(SCM, SCM);
		bool caught_error;

		// printing of basic types
		std::string prt(SCM);

		// output port
		SCM outport;

	public:
		SchemeEval(void);
		~SchemeEval();
		std::string eval(const std::string &);

		bool input_pending(void);
		void clear_pending(void);
		bool eval_error(void);
};

}

#endif/* HAVE_GUILE */
#endif /* OPENCOG_SCHEME_EVAL_H */
