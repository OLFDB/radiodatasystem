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

/*
 * rdsradiocapture \- Save songs using information from RT+ (item toggle/running/title/artist)
 *
 * Save songs using information from RT+ (MP3-ID3v2-Tags):
 * item toggle/running
 * RT+ 1: ITEM.TITLE
 * RT+ 2: ITEM.ALBUM
 * RT+ 3: ITEM.TRACKNUMBER
 * RT+ 4: ITEM.ARTIST
 * RT+ 5: ITEM.COMPOSITION
 * RT+ 6: ITEM.MOVEMENT
 * RT+ 7: ITEM.CONDUCTOR
 * RT+ 8: ITEM.COMPOSER
 * RT+ 9: ITEM.BAND
 * RT+ 10: ITEM.COMMENT
 * RT+ 11: ITEM.GENRE
 * RT+ 39: PROGRAMME.HOMEPAGE
 * RT+ 61: IDENTIFIER (according to Interational Standard Recording Code, available at http://www.ifpi.org/isrc/)
 * RT+ 62: PURCHASE
 */
