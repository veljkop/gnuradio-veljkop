/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GR_TPB_DETAIL_H
#define INCLUDED_GR_TPB_DETAIL_H

#include <boost/thread.hpp>

class gr_block_detail;

/*!
 * \brief used by thread-per-block scheduler
 */
struct gr_tpb_detail {
  typedef boost::unique_lock<boost::mutex>  scoped_lock;

  boost::mutex			mutex;			//< protects all vars
  bool				input_changed;
  boost::condition_variable	input_cond;
  bool				output_changed;
  boost::condition_variable	output_cond;

  gr_tpb_detail()
    : input_changed(false), output_changed(false) {}


  //! Called by us to tell all our upstream blocks that their output may have changed.
  void notify_upstream(gr_block_detail *d);

  //! Called by us to tell all our downstream blocks that their input may have changed.
  void notify_downstream(gr_block_detail *d);

  //! Called by us to notify both upstream and downstream
  void notify_neighbors(gr_block_detail *d);

  //! Called by us
  void clear_changed()
  {
    scoped_lock	guard(mutex);
    input_changed = false;
    output_changed = false;
  }

private:

  //! Used by notify_downstream
  void set_input_changed()
  {
    scoped_lock	guard(mutex);
    input_changed = true;
    input_cond.notify_one();
  }

  //! Used by notify_upstream
  void set_output_changed()
  {
    scoped_lock	guard(mutex);
    output_changed = true;
    output_cond.notify_one();
  }

};

#endif /* INCLUDED_GR_TPB_DETAIL_H */