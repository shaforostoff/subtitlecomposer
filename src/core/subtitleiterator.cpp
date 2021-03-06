/*
 * Copyright (C) 2007-2009 Sergio Pistone <sergio_pistone@yahoo.com.ar>
 * Copyright (C) 2010-2017 Mladen Milinkovic <max@smoothware.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "subtitleiterator.h"
#include "subtitleline.h"
#include "core/subtitle.h"

using namespace SubtitleComposer;

SubtitleIterator::SubtitleIterator(const Subtitle &subtitle, const RangeList &ranges, bool gotoLast) :
	QObject(0),
	m_subtitle(&subtitle),
	m_ranges(ranges)
{
	if(m_subtitle->isEmpty())
		m_ranges.clear();
	else
		m_ranges.trimToIndex(m_subtitle->lastIndex());

	if(m_ranges.isEmpty()) {
		m_index = Invalid;              // no operations allowed
		if(m_subtitle->linesCount())
			qDebug() << "SubtitleIterator requested with empty ranges list";
	} else {
		m_index = Invalid - 1;  // a non INVALID index (needed only for initialization)
		if(gotoLast)
			toLast();
		else
			toFirst();
	}
}

SubtitleIterator::SubtitleIterator(const SubtitleIterator &it) :
	QObject(0),
	m_subtitle(it.m_subtitle),
	m_ranges(it.m_ranges),
	m_index(it.m_index),
	m_rangesIterator(it.m_rangesIterator)
{
}

SubtitleIterator &
SubtitleIterator::operator=(const SubtitleIterator &it)
{
	if(&it != this) {

		m_subtitle = it.m_subtitle;
		m_ranges = it.m_ranges;
		m_index = it.m_index;
		m_rangesIterator = it.m_rangesIterator;
	}

	return *this;
}

SubtitleIterator::~SubtitleIterator()
{
}

RangeList
SubtitleIterator::ranges() const
{
	return m_ranges;
}

void
SubtitleIterator::toFirst()
{
	if(m_index == Invalid)
		return;

	m_rangesIterator = m_ranges.begin();
	m_index = m_ranges.firstIndex();
}

void
SubtitleIterator::toLast()
{
	if(m_index == Invalid)
		return;

	m_rangesIterator = m_ranges.end();
	m_rangesIterator--; // safe because m_ranges is not empty (m_index != Invalid)
	m_index = m_ranges.lastIndex();
}

bool
SubtitleIterator::toIndex(const int index)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index <= Range::MaxIndex);

	if(m_index == Invalid)
		return false;

	m_rangesIterator = m_ranges.begin();
	while(m_rangesIterator != m_ranges.end()) {
		if(m_rangesIterator->contains(index)) {
			m_index = index;
			return true;
		}
	}

	return false;
}

SubtitleIterator &
SubtitleIterator::operator++()
{
	if(m_index == Invalid || m_index == AfterLast)
		return *this;

	if(m_index == BehindFirst) {
		toFirst();
	} else {
		m_index++;
		if(m_index > m_rangesIterator->end()) {
			m_rangesIterator++;
			m_index = m_rangesIterator == m_ranges.end() ? AfterLast : m_rangesIterator->start();
		}
	}

	return *this;
}

SubtitleIterator &
SubtitleIterator::operator--()
{
	if(m_index == Invalid || m_index == BehindFirst)
		return *this;

	if(m_index == AfterLast) {
		toLast();
	} else {
		m_index--;
		if(m_index < m_rangesIterator->start()) {
			m_index = m_rangesIterator == m_ranges.begin() ? BehindFirst : m_rangesIterator->end();
			m_rangesIterator--;
		}
	}

	return *this;
}

SubtitleIterator &
SubtitleIterator::operator+=(int steps)
{
	if(steps < 0)
		return operator-=(-steps);

	if(m_index == Invalid || m_index == AfterLast)
		return *this;

	while(steps--)
		operator++();

	return *this;
}

SubtitleIterator &
SubtitleIterator::operator-=(int steps)
{
	if(steps < 0)
		return operator+=(-steps);

	if(m_index == Invalid || m_index == BehindFirst)
		return *this;

	while(steps--)
		operator--();

	return *this;
}
