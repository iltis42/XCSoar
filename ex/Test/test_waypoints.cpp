#include "harness_waypoints.hpp"
#include "test_debug.hpp"

#include "Waypoint/WaypointVisitor.hpp"

class WaypointVisitorPrint: public WaypointVisitor {
public:
  WaypointVisitorPrint():count(0) {};

  virtual void Visit(const Waypoint& wp) {
    if (verbose) {
#ifdef DO_PRINT
      printf("# visiting wp %d, '%s'\n", wp.id, wp.Name.c_str());
#endif
    }
    count++;
  }
  unsigned count;
  void reset() {
    count = 0;
  }
};

unsigned test_range(const Waypoints& waypoints, const double range)
{
  const Waypoint *r = waypoints.lookup_id(3);
  if (r) {
    WaypointVisitorPrint v;
    waypoints.visit_within_range(r->Location, range, v);
    return v.count;
  } else {
    return 0;
  }
}

unsigned test_radius(const Waypoints& waypoints, const double range)
{
  const Waypoint *r = waypoints.lookup_id(3);
  if (r) {
    WaypointVisitorPrint v;
    waypoints.visit_within_radius(r->Location, range, v);
    return v.count;
  } else {
    return 0;
  }
}

unsigned test_nearest(const Waypoints& waypoints)
{
  const Waypoint *r = waypoints.lookup_id(3);
  if (r) {
    Waypoints::WaypointTree::const_iterator it = waypoints.find_nearest(r->Location);
    if (it != waypoints.end()) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

unsigned test_copy(Waypoints& waypoints)
{
  const Waypoint *r = waypoints.lookup_id(3);
  if (!r) {
    return false;
  }
  unsigned size_old = waypoints.size();
  Waypoint wp = *r;
  waypoints.insert(wp);
  waypoints.optimise();
  unsigned size_new = waypoints.size();
  return (size_new == size_old+1);
}

bool test_lookup(const Waypoints& waypoints, unsigned id)
{
  const Waypoint* wp;
  wp = waypoints.lookup_id(id);
  if (wp== NULL) {
    return false;
  }
  return true;
}

int main(int argc, char** argv)
{
  ::InitSineTable();

  if (!parse_args(argc,argv)) {
    return 0;
  }

  plan_tests(11);

  Waypoints waypoints;

  ok(setup_waypoints(waypoints),"waypoint setup",0);

  unsigned size = waypoints.size();

  ok(test_lookup(waypoints,3),"waypoint lookup",0);
  ok(!test_lookup(waypoints,5000),"waypoint bad lookup",0);
  ok(test_nearest(waypoints),"waypoint nearest",0);
  ok(test_range(waypoints,100)==1,"waypoint visit range 100m",0);
  ok(test_radius(waypoints,100)==1,"waypoint radius 100m",0);
  ok(test_range(waypoints,500000)== waypoints.size(),"waypoint range 500000m",0);
  ok(test_radius(waypoints,25000)<= test_range(waypoints,25000),"waypoint radius<range",0);

  // test clear
  waypoints.clear();
  ok(waypoints.size()==0,"waypoint clear",0);
  setup_waypoints(waypoints);
  ok(size == waypoints.size(),"waypoint setup after clear",0);

  ok(test_copy(waypoints),"waypoint copy",0);

  return exit_status();
}
