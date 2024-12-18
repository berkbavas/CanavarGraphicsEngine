import "EFIS/EADI"
import "EFIS/EHSI"

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    width: 300
    height: 300

    anchors.centerIn: parent

    ElectronicAttitudeDirectionIndicator {
        anchors.fill: parent

        adi.angleOfAttack: pfd.angleOfAttack
        adi.sideSlipAngle: pfd.angleOfSideSlip
        adi.roll: pfd.roll
        adi.pitch: pfd.pitch
        adi.slipSkid: pfd.slipSkid
        adi.turnRate: pfd.turnRate
        adi.dotH: pfd.ilsLOC
        adi.dotV: pfd.ilsGS
        adi.fdPitch: pfd.fdPitch
        adi.fdRoll: pfd.fdRoll
        adi.dotHVisible: pfd.ilsLOCVisible
        adi.dotVVisible: pfd.ilsGSVisible
        adi.fdVisible: pfd.fdVisible
        adi.stallVisible: pfd.stall

        asi.airspeed: pfd.airspeed
        asi.bugValue: pfd.airspeedBug

        alt.altitude: pfd.altitude
        alt.bugValue: pfd.altitudeBug

        hsi.heading: pfd.heading
        hsi.bugValue: pfd.headingBug

        vsi.climbRate: pfd.climbRate

        labels.airspeedBug: pfd.airspeedBug
        labels.machNumber: pfd.machNumber
        labels.altitudeBug: pfd.altitudeBug
        labels.pressure: pfd.pressure
        labels.pressureMode: pfd.pressureMode
        labels.flightMode: pfd.flightMode
        labels.speedMode: pfd.speedMode
        labels.lnav: pfd.lateralNavigationMode
        labels.vnav: pfd.verticalNavigationMode
    }
}
