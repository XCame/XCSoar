/* Generated by Together */

#include "AbstractTask.h"
#include "Navigation/Aircraft.hpp"
#include "BaseTask/TaskPoint.hpp"
#include "TaskSolvers/TaskBestMc.hpp"
#include "TaskSolvers/TaskGlideRequired.hpp"
#include "Util.h"
#include <iostream>
#include <fstream>

bool 
AbstractTask::update_idle(const AIRCRAFT_STATE &state)
{
  double mc= 2.0; // TODO hard coded!
  if (1) {
    stats.mc_best = calc_mc_best(state, mc);
    stats.cruise_efficiency = calc_cruise_efficiency(state, mc);
  } else {
    stats.mc_best = mc;
    stats.cruise_efficiency = 1.0;
  }
  // do nothing
  return false;
}


unsigned 
AbstractTask::getActiveTaskPointIndex() 
{
  return activeTaskPoint;
}

void
AbstractTask::update_stats_distances(const GEOPOINT &location,
                                     const bool full_update)
{

  stats.total.remaining.set_distance(scan_distance_remaining(location));

  if (full_update) {
    stats.distance_nominal = scan_distance_nominal();
  }

  scan_distance_minmax(location, 
                       full_update,
                       &stats.distance_min,
                       &stats.distance_max);

  stats.total.travelled.set_distance(scan_distance_travelled(location));
  stats.total.planned.set_distance(scan_distance_planned());

  stats.distance_scored = scan_distance_scored(location);

}

void
AbstractTask::update_glide_solutions(const AIRCRAFT_STATE &state,
                                     const double mc)
{
  glide_solution_remaining(state, mc,
                           stats.total.solution_remaining,
                           stats.current_leg.solution_remaining);

  glide_solution_travelled(state, mc,
                           stats.total.solution_travelled,
                           stats.current_leg.solution_travelled);

  glide_solution_planned(state, mc,
                         stats.total.solution_planned,
                         stats.current_leg.solution_planned,
                         stats.total.remaining_effective,
                         stats.current_leg.remaining_effective,
                         stats.total.solution_remaining.TimeElapsed,
                         stats.current_leg.solution_remaining.TimeElapsed);

  stats.current_leg.remaining.set_distance(
    stats.current_leg.solution_remaining.Distance);
  stats.current_leg.travelled.set_distance(
    stats.current_leg.solution_travelled.Distance);
  stats.current_leg.planned.set_distance(
    stats.current_leg.solution_planned.Distance);

  stats.total.gradient = ::AngleToGradient(calc_gradient(state));
  stats.current_leg.gradient = ::AngleToGradient(leg_gradient(state));
}

bool
AbstractTask::update(const AIRCRAFT_STATE &state, 
                     const AIRCRAFT_STATE &state_last)
{
  bool retval;
  double mc= 2.0; // TODO hard coded!

  const bool full_update = check_transitions(state, state_last);

  update_stats_times(state);

  update_stats_distances(state.Location, full_update);

  update_glide_solutions(state, mc);

  retval = update_sample(state, full_update);

  update_stats_glide(state, mc);

  update_stats_speeds(state, state_last);
  return retval;
}

void
AbstractTask::update_stats_speeds(const AIRCRAFT_STATE &state, 
                                  const AIRCRAFT_STATE &state_last)
{
  const double dt = state.Time-state_last.Time;
  stats.total.calc_speeds(dt);
  stats.current_leg.calc_speeds(dt);
}

void
AbstractTask::update_stats_glide(const AIRCRAFT_STATE &state, 
                                 const double mc)
{
  stats.glide_required = AngleToGradient(calc_glide_required(state));
}

void
AbstractTask::update_stats_times(const AIRCRAFT_STATE &state)
{
  // default for tasks with no start time...
  stats.total.set_times(scan_total_start_time(state), state);
  stats.current_leg.set_times(scan_leg_start_time(state),state);
}


void 
AbstractTask::scan_distance_minmax(const GEOPOINT &location, bool full,
                                    double *dmin, double *dmax)
{
  *dmin = stats.total.remaining.get_distance();
  *dmax = stats.total.remaining.get_distance();
}

double 
AbstractTask::scan_distance_nominal()
{
  return stats.total.remaining.get_distance();
}

double 
AbstractTask::scan_distance_planned()
{
  return stats.total.remaining.get_distance();
}

double 
AbstractTask::scan_distance_scored(const GEOPOINT &location)
{
  return 0.0;
}

double 
AbstractTask::scan_distance_travelled(const GEOPOINT &location)
{
  return 0.0;
}

double 
AbstractTask::scan_distance_remaining(const GEOPOINT &location)
{
  TaskPoint *tp = getActiveTaskPoint();
  if (!tp) {
    return 0.0;
  }
  return tp->distance(location);
}


void
AbstractTask::glide_solution_remaining(const AIRCRAFT_STATE &state, 
                                       const double mc,
                                       GLIDE_RESULT &total,
                                       GLIDE_RESULT &leg)
{
  GLIDE_RESULT res;
  MacCready msolv; // TODO make this common
  msolv.set_mc(mc);

  TaskPoint* tp = getActiveTaskPoint();
  if (tp) {
    res = tp->glide_solution_remaining(state, msolv, 0.0);
  }
  total = res;
  leg = res;
}

void 
AbstractTask::glide_solution_travelled(const AIRCRAFT_STATE &state, 
                                       const double mc, 
                                       GLIDE_RESULT &total,
                                       GLIDE_RESULT &leg)
{
  GLIDE_RESULT null_res;
  total = null_res;
  leg = null_res;
}

void 
AbstractTask::glide_solution_planned(const AIRCRAFT_STATE &state, 
                                     const double mc, 
                                     GLIDE_RESULT &total,
                                     GLIDE_RESULT &leg,
                                     DistanceRemainingStat &total_remaining_effective,
                                     DistanceRemainingStat &leg_remaining_effective,
                                     const double total_t_elapsed,
                                     const double leg_t_elapsed)
{
  GLIDE_RESULT res = stats.total.solution_remaining;
  total = res;
  leg = res;
  total_remaining_effective.set_distance(res.Distance);
  leg_remaining_effective.set_distance(res.Distance);
}

void
AbstractTask::report(const AIRCRAFT_STATE &state)
{
  std::ofstream fs("res-stats-all.txt");
  stats.print(fs);

  static std::ofstream f4("res-sample.txt");
  f4 <<  state.Location.Longitude << " " 
     <<  state.Location.Latitude << "\n";
  f4.flush();

  static std::ofstream f6("res-stats.txt");
  static bool first = true;

  if (first) {
    first = false;
    f6 << "# Time atp mc_best dist_rem_eff dist_rem cruis_eff sir sire\n";
  }
  f6 << state.Time
     << " " << activeTaskPoint
     << " " << stats.mc_best
     << " " << stats.total.remaining_effective.get_distance()
     << " " << stats.total.remaining.get_distance() 
     << " " << stats.cruise_efficiency 
     << " " << stats.total.remaining.get_speed() 
     << " " << stats.total.remaining.get_speed_incremental() 
     << " " << stats.total.remaining_effective.get_speed() 
     << " " << stats.total.remaining_effective.get_speed_incremental() 
     << "\n";
  f6.flush();

}


double 
AbstractTask::scan_total_start_time(const AIRCRAFT_STATE &state)
{
  return state.Time;
}

double 
AbstractTask::scan_leg_start_time(const AIRCRAFT_STATE &state)
{
  return state.Time;
}

double 
AbstractTask::calc_mc_best(const AIRCRAFT_STATE &aircraft, 
                           const double mc)
{
  TaskPoint *tp = getActiveTaskPoint();
  if (!tp) {
    return mc;
  }
  TaskBestMc bmc(tp, aircraft);
  return bmc.search(mc);
}

double 
AbstractTask::calc_glide_required(const AIRCRAFT_STATE &aircraft)
{
  TaskPoint *tp = getActiveTaskPoint();
  if (!tp) {
    return 0.0;
  }
  TaskGlideRequired bgr(tp, aircraft);
  return bgr.search(0.0);
}


double
AbstractTask::leg_gradient(const AIRCRAFT_STATE &aircraft) 
{
  TaskPoint *tp = getActiveTaskPoint();
  if (!tp) {
    return 0.0;
  }
  const double d = tp->get_distance_remaining(aircraft);
  if (d) {
    return (aircraft.Altitude-tp->getElevation())/d;
  } else {
    return 0.0;
  }
}

double 
AbstractTask::calc_gradient(const AIRCRAFT_STATE &state) 
{
  return leg_gradient(state);
}
