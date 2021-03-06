// Copyright (c) 2014, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of hpp-manipulation.
// hpp-manipulation is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-manipulation is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-manipulation. If not, see <http://www.gnu.org/licenses/>.

#include "hpp/manipulation/graph-steering-method.hh"

#include <hpp/util/pointer.hh>

#include <hpp/core/straight-path.hh>
#include <hpp/core/steering-method/straight.hh>

#include <hpp/manipulation/problem.hh>
#include <hpp/manipulation/graph/graph.hh>
#include <hpp/manipulation/graph/edge.hh>

namespace hpp {
  namespace manipulation {
    GraphSteeringMethodPtr_t GraphSteeringMethod::create
      (const core::Problem& problem)
    {
      dynamic_cast <const Problem&> (problem);
      const Problem& p = static_cast <const Problem&> (problem);
      return create (p);
    }

    GraphSteeringMethodPtr_t GraphSteeringMethod::create
    (const Problem& problem)
    {
      GraphSteeringMethod* ptr = new GraphSteeringMethod (problem);
      GraphSteeringMethodPtr_t shPtr (ptr);
      ptr->init (shPtr);
      return shPtr;
    }

    GraphSteeringMethodPtr_t GraphSteeringMethod::createCopy
    (const GraphSteeringMethodPtr_t& other)
    {
      GraphSteeringMethod* ptr = new GraphSteeringMethod (*other);
      GraphSteeringMethodPtr_t shPtr (ptr);
      ptr->init (shPtr);
      return shPtr;
    }

    GraphSteeringMethod::GraphSteeringMethod (const Problem& problem) :
      SteeringMethod (problem), problem_ (problem), weak_ (),
      steeringMethod_ (core::SteeringMethodStraight::create (problem))
    {
    }

    GraphSteeringMethod::GraphSteeringMethod (const GraphSteeringMethod& other):
      SteeringMethod (other), problem_ (other.problem_), steeringMethod_
      (other.steeringMethod_)
    {
    }

    PathPtr_t GraphSteeringMethod::impl_compute (ConfigurationIn_t q1, ConfigurationIn_t q2) const
    {
      graph::Edges_t possibleEdges;
      const graph::Graph& graph = *problem_.constraintGraph ();
      try {
        possibleEdges = graph.getEdges
	  (graph.getState (q1), graph.getState (q2));
      } catch (const std::logic_error& e) {
        hppDout (error, e.what ());
        return PathPtr_t ();
      }
      PathPtr_t path;
      if (possibleEdges.empty()) {
        hppDout (info, "No edge found.");
      }
      while (!possibleEdges.empty()) {
        if (possibleEdges.back ()->build (path, q1, q2)) {
          return path;
        }
        possibleEdges.pop_back ();
      }
      return PathPtr_t ();
    }
  } // namespace manipulation
} // namespace hpp
