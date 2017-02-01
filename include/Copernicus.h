#pragma once
#ifndef COPERNICUS_H
#define COPERNICUS_H

namespace titan {
	void setTurretAngle(const float angle);

	void setFloorIntake(const bool floorIntake);

	void setTopIntake(const bool topIntake);

	void setHighGear(const bool highGear);

	void setGearIn(const bool gear);

	void setFlywheelRPM(const float rpm);

	void setTargetRPM(const float rpm);
}

#endif