/*
 * Copyright (c) 2009 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "HMSSpinBox.h"

static const int SECONDS_IN_MINUTE = 60;
static const int SECONDS_IN_HOUR = 60 * SECONDS_IN_MINUTE;
static const int SECONDS_IN_DAY = 24 * SECONDS_IN_HOUR;

HMSSpinBox::HMSSpinBox(QWidget*parent)
: QSpinBox(parent)
{
}

QString HMSSpinBox::textFromValue(int value) const
{
      int days = 0;
      int hours = 0;
      int minutes = 0;

      if (value >= SECONDS_IN_DAY) {
	    days = value / SECONDS_IN_DAY;
	    value %= SECONDS_IN_DAY;
      }

      if (value >= SECONDS_IN_HOUR) {
	    hours = value / SECONDS_IN_HOUR;
	    value %= SECONDS_IN_HOUR;
      }

      if (value >= SECONDS_IN_MINUTE) {
	    minutes = value / SECONDS_IN_MINUTE;
	    value %= SECONDS_IN_MINUTE;
      }

      return QString("%L1d %2:%3:%4")
	    .arg(days)
	    .arg(hours,2,10,QLatin1Char('0'))
	    .arg(minutes,2,10,QLatin1Char('0'))
	    .arg(value,2,10,QLatin1Char('0'));
}

int HMSSpinBox::valueFromText(const QString&text) const
{
      int days = 0;
      int hours = 0;
      int minutes = 0;
      int seconds = 0;

      QStringList parts = text.split(QChar(' '), QString::SkipEmptyParts);
      QString hours_text;
      if (parts.size() >= 2) {
	    hours_text = parts[1];
	    QString days_text = parts[0];

	    assert(days_text.endsWith(QChar('d')));
	    days_text.chop(1);
	    days = days_text.toInt();

      } else {
	    hours_text = parts[0];
      }

      parts = hours_text.split(QChar(':'));
      assert(parts.size() == 3);

      hours = parts[0].toInt();
      minutes = parts[1].toInt();
      seconds = parts[2].toInt();

      return days*SECONDS_IN_DAY
	    + hours*SECONDS_IN_HOUR
	    + minutes*SECONDS_IN_MINUTE
	    + seconds;
}

QValidator::State HMSSpinBox::validate(QString&input, int&pos) const
{
      QStringList parts = input.split(QChar(' '), QString::SkipEmptyParts);
      if (parts.size() > 2)
	    return QValidator::Invalid;

      QString hours_text;
      if (parts.size() == 2) {
	    hours_text = parts[1];
	    QString days_text = parts[0];

	    if (!days_text.endsWith(QChar('d')))
		  return QValidator::Invalid;

      } else {
	    hours_text = parts[0];
      }

      parts = hours_text.split(QChar(':'));
      if (parts.size() != 3)
	    return QValidator::Invalid;

      return QValidator::Acceptable;
}
