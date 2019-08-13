/*

This file is part of VROOM.

Copyright (c) 2015-2019, Julien Coupey.
All rights reserved (see LICENSE).

*/

#include <numeric>

#include "structures/vroom/job.h"
#include "utils/exception.h"

namespace vroom {

Amount delivery_mask(const Amount& amount) {
  Amount d(amount.size());

  for (std::size_t i = 0; i < amount.size(); ++i) {
    if (amount[i] > 0) {
      d[i] = amount[i];
    }
  }

  return d;
}

Amount pickup_mask(const Amount& amount) {
  Amount p(amount.size());

  for (std::size_t i = 0; i < amount.size(); ++i) {
    if (amount[i] < 0) {
      p[i] = -amount[i];
    }
  }

  return p;
}

Job::Job(Id id,
         const Location& location,
         Duration service,
         const Amount& amount,
         const Skills& skills,
         Priority priority,
         const std::vector<TimeWindow>& tws)
  : location(location),
    id(id),
    service(service),
    amount(amount),
    delivery(delivery_mask(amount)),
    pickup(pickup_mask(amount)),
    skills(skills),
    priority(priority),
    tws(tws),
    tw_length(
      std::accumulate(std::next(tws.begin()),
                      tws.end(),
                      tws[0].length,
                      [](auto sum, auto tw) { return sum + tw.length; })) {
  if (tws.size() == 0) {
    throw Exception(ERROR::INPUT,
                    "Empty time-windows for job " + std::to_string(id) + ".");
  }

  if (tws.size() > 1) {
    for (std::size_t i = 0; i < tws.size() - 1; ++i) {
      if (tws[i + 1].start <= tws[i].end) {
        throw Exception(ERROR::INPUT,
                        "Unsorted or overlapping time-windows for job " +
                          std::to_string(id) + ".");
      }
    }
  }
}

bool Job::is_valid_start(Duration time) const {
  bool valid = false;

  for (const auto& tw : tws) {
    if (tw.contains(time)) {
      valid = true;
      break;
    }
  }

  return valid;
}

} // namespace vroom
