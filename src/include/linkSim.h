/* File: linkSim.h
 * Description: A header file for the link level simulator
 * Copyright (C) 2011-2014, Jonghun John Park
 * Last updated on Dec. 12, 2014
 *
 * This file is part of ComSim.
 * ComSim is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ComSim. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef __LINKSIM_H__
#define __LINKSIM_H__

/* Defines */

/* Headers */
#include "comSim_types.h"

/* Data Structures */
/* extern simParm_str linkSimParam;
 * extern dataPath_str dataPath;
 */

/* Global Variables */
char *param_file_default = "param_linkSim.dat";

/* Functions */
int linkSim_init();
int linkSim_update(double snr);
int linkSim_countErr();
int linkSim_summary(double snr);

#endif /* __LINKSIM_H__ */
