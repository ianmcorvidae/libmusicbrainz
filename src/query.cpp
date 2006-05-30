/*
 * MusicBrainz -- The Internet music metadatabase
 *
 * Copyright (C) 2006 Lukas Lalinsky
 *	
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	 02111-1307	 USA
 */
 
#include <string>
#include <iostream>
#include <musicbrainz3/utils.h>
#include <musicbrainz3/query.h>
#include <musicbrainz3/mbxmlparser.h>

using namespace std;
using namespace MusicBrainz;

Query::Query(IWebService *ws, const string &clientId)
	: ws(ws), clientId(clientId)
{
}

Query::~Query()
{
}

Artist *
Query::getArtistById(const string &id,
					 const ArtistIncludes &include)
{
	string uuid = extractUuid(id);
	Metadata *metadata = getFromWebService<ArtistIncludes, ArtistFilter>("artist", uuid, include); 
	Artist *artist = metadata->getArtist(true);
	delete metadata;
	return artist;
}

Release *
Query::getReleaseById(const string &id,
					 const ReleaseIncludes &include)
{
	string uuid = extractUuid(id);
	Metadata *metadata = getFromWebService<ReleaseIncludes, ReleaseFilter>("release", uuid, include); 
	Release *release = metadata->getRelease(true);
	delete metadata;
	return release;
}

Track *
Query::getTrackById(const string &id,
					 const TrackIncludes &include)
{
	string uuid = extractUuid(id);
	Metadata *metadata = getFromWebService<TrackIncludes, TrackFilter>("track", uuid, include); 
	Track *track = metadata->getTrack(true);
	delete metadata;
	return track;
}

User *
Query::getUserByName(const string &name)
{
	Metadata *metadata = getFromWebService<TrackIncludes, UserFilter>("user", "", TrackIncludes(), UserFilter().name(name));
	UserList &list = metadata->getUserList(true);
	delete metadata;
	if (list.size() > 0) 
		return list[0];
	else
		throw ResponseError("response didn't contain user data");
}

template<typename IT, typename FT>
Metadata *
Query::getFromWebService(const string &entity,
						 const string &id,
						 const IT &include,
						 const FT &filter)
{
	const IIncludes::IncludeList includeParams(include.createIncludeTags());
	const IFilter::ParameterList filterParams(filter.createParameters());
	string content = ws->get(entity, id, includeParams, filterParams);
	try {
		MbXmlParser parser;
		return parser.parse(content);
	}
	catch (ParseError &e) {
		throw ResponseError(e.what());
	}
}
