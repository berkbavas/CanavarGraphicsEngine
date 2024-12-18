import "BasicSix"

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.0

Grid {
    id: root
    columns: 7
    columnSpacing: 16
    rowSpacing: 16

    property double radius: 120

    anchors {
        margins: 32
    }

    AirspeedIndicator {
        radius: root.radius
        airspeed: pfd.airspeed
    }

    AttitudeIndicator {
        radius: root.radius
        roll: pfd.roll
        pitch: pfd.pitch
    }

    Altimeter {
        radius: root.radius
        altitude: pfd.altitude
        pressure: pfd.pressure
    }
    
    Item {
        width: 240
        height: 240
    }

    TurnCoordinator {
        radius: root.radius
        turnRate: pfd.turnRate
        slipSkid: pfd.slipSkid
    }

    HeadingIndicator {
        radius: root.radius
        heading: pfd.heading
    }

    VerticalSpeedIndicator {
        radius: root.radius
        climbRate: pfd.climbRate
    }
}