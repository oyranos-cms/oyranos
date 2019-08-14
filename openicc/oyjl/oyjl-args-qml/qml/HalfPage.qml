/** @file HalfPage.qml
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/06/08
 *
 *  Flexible page with half wide in landscape orientation. Otherwise it is full width.
 *
 *  The Component needs:
 *  - 'pages' rectangle around to take width and height dimensions from.
 *  - 'landscape' boolean variable
 *  - 'bg' background color
 */

import QtQuick 2.7
import QtQuick.Controls 2.4

Rectangle {
    width: {
        if(landscape)
            pages.width/2
        else
            pages.width
    }
    height: pages.height
    color: bg
}
