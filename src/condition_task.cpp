#include "ppxbase/condition_task.h"
#include <agents.h>

namespace ppx {
    namespace base {

        Concurrency::task<void> RunAfter(unsigned int timeout_ms) {
            Concurrency::task_completion_event<void> tce;

            auto fire_once = new Concurrency::timer<int>(timeout_ms, 0, nullptr, false);

            auto callback = new Concurrency::call<int>([tce](int) {
                tce.set();
            });


            fire_once->link_target(callback);
            fire_once->start();

            //Concurrency::task_options opt;
            //opt.set_cancellation_token(cts.get_token());

            //Concurrency::task<void> event_set(tce, opt);
            Concurrency::task<void> event_set(tce);

            return event_set.then([callback, fire_once]() {
                delete callback;
                delete fire_once;
            });
        }

    }
}