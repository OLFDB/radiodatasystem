/*
 * Copyright © 2009-2011 Tobias Lorenz
 *
 * This file is part of librds.
 *
 * librds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librds is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with librds.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file doxygen.h
 * \brief Doxygen documentation
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains no functions, just doxygen comments.
 */

/**

\mainpage Radio Data System decoder library

\author Tobias Lorenz

\page databases Databases


\section db_rds_freq RDS Frequency Database
Default location: $DATADIR/rds_freq_*.db

Tables: non defined yet.


\section db_rds_lang RDS Language Database
Default location: $SHAREDSTATEDIR/rds_lang_*.db

Tables:
- CC: (Extended) Country Code Table
- LIC: Language Identification Code Table
- MS: Music/Speech Table
- PTY: Programme Type Table
- RTP: Radio Text Plus


\section db_rds_program RDS Program Database
Default location: $DATADIR/rds_program.db

Tables:
- RDS: Radio Data System Table


\section db_tmc_dsp TMC Data Service Provider Database
Default location: $DATADIR/tmc_dsp.db

Tables:
- ALTERNATIVES: Alternatives
- ENCRYPTION: Encryption
- SERVICE: Service
- TIMING: Timing


\section db_tmc_el TMC Event List Database
Default location: $SHAREDSTATEDIR/tmc_el_*.db

Tables:
- EL: Event List
- FEL: Forecase Event List
- PC: Phrase Codes
- QC: Quantifier Codes
- SIL: Supplementary Information List


\section db_tmc_lcl TMC Location Code List Database
Default location: $SHAREDSTATEDIR/tmc_lcl_*.db

Tables as defined by the TMC Forum Exchange Format


\section db_tmc_skt TMC Service Key Table Database
Default location: $SHAREDSTATEDIR/tmc_skt_*.db

Tables: non defined yet.
*/
