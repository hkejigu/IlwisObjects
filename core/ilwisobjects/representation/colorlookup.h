/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef COLORLOOKUP_H
#define COLORLOOKUP_H

namespace Ilwis {

class Domain;
typedef IlwisData<Domain> IDomain;


class NumericRange;

class KERNELSHARED_EXPORT ColorLookUp
{
public:
    virtual QColor value2color(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const = 0;
    virtual void setColor(double value, const QColor& clr)  = 0;
    std::vector<QColor> values2colors(const NumericRange &actualRange, const NumericRange &stretchRange, int paletteSize=256) const;
    virtual ColorLookUp *clone() const = 0;
    static ColorLookUp *create(const Ilwis::IDomain &dom, const QString &rprCode="");
protected:
    QColor string2color(const QString &colorstring);
};




}

#endif // COLORLOOKUP_H
