/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/


// Flags are defined and declared using DEFINE_xxx and DECLARE_xxx macros,
// where xxx is the flag type. Flags are referred to via FLAG_yyy,
// where yyy is the flag name. For intialization and iteration of flags,
// see the FlagList class. For full programmatic access to any
// flag, see the Flag class.
//
// The implementation only relies and basic C++ functionality
// and needs no special library or STL support.

#ifndef PPX_BASE_FLAGS_H_
#define PPX_BASE_FLAGS_H_

#include "ppxbase/assert.h"
#include "ppxbase/constructormagic.h"

namespace ppx {
    namespace base {
        // Internal use only.
        union FlagValue {
            // Note: Because in C++ non-bool values are silently converted into
            // bool values ('bool b = "false";' results in b == true!), we pass
            // and int argument to New_BOOL as this appears to be safer - sigh.
            // In particular, it prevents the (not uncommon!) bug where a bool
            // flag is defined via: DEFINE_bool(flag, "false", "some comment");.
            static FlagValue New_BOOL(int b) {
                FlagValue v;
                v.b = (b != 0);
                return v;
            }

            static FlagValue New_INT(int i) {
                FlagValue v;
                v.i = i;
                return v;
            }

            static FlagValue New_FLOAT(float f) {
                FlagValue v;
                v.f = f;
                return v;
            }

            static FlagValue New_STRING(const char *s) {
                FlagValue v;
                v.s = s;
                return v;
            }

            bool b;
            int i;
            double f;
            const char *s;
        };


        // Each flag can be accessed programmatically via a Flag object.
        class Flag {
          public:
            enum Type { BOOL, INT, FLOAT, STRING };

            // Internal use only.
            Flag(const char *file, const char *name, const char *comment,
                 Type type, void *variable, FlagValue default_);

            // General flag information
            const char *file() const {
                return file_;
            }
            const char *name() const {
                return name_;
            }
            const char *comment() const {
                return comment_;
            }

            // Flag type
            Type type() const {
                return type_;
            }

            // Flag variables
            bool *bool_variable() const {
                PPX_ASSERT(BOOL == type_);
                return &variable_->b;
            }

            int *int_variable() const {
                PPX_ASSERT(INT == type_);
                return &variable_->i;
            }

            double *float_variable() const {
                PPX_ASSERT(FLOAT == type_);
                return &variable_->f;
            }

            const char **string_variable() const {
                PPX_ASSERT(STRING == type_);
                return &variable_->s;
            }

            // Default values
            bool bool_default() const {
                PPX_ASSERT(BOOL == type_);
                return default_.b;
            }

            int int_default() const {
                PPX_ASSERT(INT == type_);
                return default_.i;
            }

            double float_default() const {
                PPX_ASSERT(FLOAT == type_);
                return default_.f;
            }

            const char *string_default() const {
                PPX_ASSERT(STRING == type_);
                return default_.s;
            }

            // Resets a flag to its default value
            void SetToDefault();

            // Iteration support
            Flag *next() const {
                return next_;
            }

            // Prints flag information. The current flag value is only printed
            // if print_current_value is set.
            void Print(bool print_current_value);

          private:
            const char *file_;
            const char *name_;
            const char *comment_;

            Type type_;
            FlagValue *variable_;
            FlagValue default_;

            Flag *next_;

            friend class FlagList;  // accesses next_
        };


        // Internal use only.
#define DEFINE_FLAG(type, c_type, name, default, comment) \
    /* define and initialize the flag */                    \
    c_type FLAG_##name = (default);                         \
    /* register the flag */                                 \
    static ppx::base::Flag Flag_##name(__FILE__, #name, (comment),      \
                                 ppx::base::Flag::type, &FLAG_##name,   \
                                 ppx::base::FlagValue::New_##type(default))


        // Internal use only.
#define DECLARE_FLAG(c_type, name)              \
    /* declare the external flag */               \
    extern c_type FLAG_##name


        // Use the following macros to define a new flag:
#define DEFINE_bool(name, default, comment) \
    DEFINE_FLAG(BOOL, bool, name, default, comment)
#define DEFINE_int(name, default, comment) \
    DEFINE_FLAG(INT, int, name, default, comment)
#define DEFINE_float(name, default, comment) \
    DEFINE_FLAG(FLOAT, double, name, default, comment)
#define DEFINE_string(name, default, comment) \
    DEFINE_FLAG(STRING, const char*, name, default, comment)


        // Use the following macros to declare a flag defined elsewhere:
#define DECLARE_bool(name)  DECLARE_FLAG(bool, name)
#define DECLARE_int(name)  DECLARE_FLAG(int, name)
#define DECLARE_float(name)  DECLARE_FLAG(double, name)
#define DECLARE_string(name)  DECLARE_FLAG(const char*, name)


        // The global list of all flags.
        class FlagList {
          public:
            FlagList();

            // The null-terminated list of all flags. Traverse with Flag::next().
            static Flag *list() {
                return list_;
            }

            // If file != nullptr, prints information for all flags defined in file;
            // otherwise prints information for all flags in all files. The current flag
            // value is only printed if print_current_value is set.
            static void Print(const char *file, bool print_current_value);

            // Lookup a flag by name. Returns the matching flag or null.
            static Flag *Lookup(const char *name);

            // Helper function to parse flags: Takes an argument arg and splits it into
            // a flag name and flag value (or null if they are missing). is_bool is set
            // if the arg started with "-no" or "--no". The buffer may be used to NUL-
            // terminate the name, it must be large enough to hold any possible name.
            static void SplitArgument(const char *arg,
                                      char *buffer, int buffer_size,
                                      const char **name, const char **value,
                                      bool *is_bool);

            // Set the flag values by parsing the command line. If remove_flags
            // is set, the flags and associated values are removed from (argc,
            // argv). Returns 0 if no error occurred. Otherwise, returns the
            // argv index > 0 for the argument where an error occurred. In that
            // case, (argc, argv) will remain unchanged indepdendent of the
            // remove_flags value, and no assumptions about flag settings should
            // be made.
            //
            // The following syntax for flags is accepted (both '-' and '--' are ok):
            //
            //   --flag        (bool flags only)
            //   --noflag      (bool flags only)
            //   --flag=value  (non-bool flags only, no spaces around '=')
            //   --flag value  (non-bool flags only)
            static int SetFlagsFromCommandLine(int *argc,
                                               const char **argv,
                                               bool remove_flags);
            static inline int SetFlagsFromCommandLine(int *argc,
                    char **argv,
                    bool remove_flags) {
                return SetFlagsFromCommandLine(argc, const_cast<const char **>(argv),
                                               remove_flags);
            }

            // Registers a new flag. Called during program initialization. Not
            // thread-safe.
            static void Register(Flag *flag);

          private:
            static Flag *list_;
        };


        // A helper class to translate Windows command line arguments into UTF8,
        // which then allows us to just pass them to the flags system.
        // This encapsulates all the work of getting the command line and translating
        // it to an array of 8-bit strings; all you have to do is create one of these,
        // and then call argc() and argv().
        class WindowsCommandLineArguments {
          public:
            WindowsCommandLineArguments();
            ~WindowsCommandLineArguments();

            int argc() {
                return argc_;
            }
            char **argv() {
                return argv_;
            }
          private:
            int argc_;
            char **argv_;

          private:
            PPX_DISALLOW_COPY_AND_ASSIGN(WindowsCommandLineArguments);
        };

    }
}

#endif  // PPX_BASE_FLAGS_H_
