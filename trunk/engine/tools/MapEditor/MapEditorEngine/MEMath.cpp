#include "stdafx.h"
#include "MEMath.h"
#include "CPlane.h"
#include "Float.h"
namespace sqr
{
	BOOL IntersectedPlane(CVector3f vPoly[], CVector3f vLine[], CVector3f &vNormal, float &originDistance)
	{
		float distance1=0, distance2=0;	// The distances from the 2 points of the line from the plane

		CVector3f vA = vPoly[2] - vPoly[0];
		CVector3f vB = vPoly[1] - vPoly[0];

		vNormal = vA.Cross(vB);		// Take the cross product of our 2 vectors to get a perpendicular vector
		vNormal.normalize();

		// Now we have a normal, but it's at a strange length, so let's make it length 1.

		// Let's find the distance our plane is from the origin.  We can find this value
		// from the normal to the plane (polygon) and any point that lies on that plane (Any vertex)
		originDistance = PlaneDistance(vNormal, vPoly[0]);

		// Get the distance from point1 from the plane using: Ax + By + Cz + D = (The distance from the plane)

		distance1 = ((vNormal.x * vLine[0].x)  +					// Ax +
					 (vNormal.y * vLine[0].y)  +					// Bx +
					 (vNormal.z * vLine[0].z)) + originDistance;	// Cz + D

		// Get the distance from point2 from the plane using Ax + By + Cz + D = (The distance from the plane)

		distance2 = ((vNormal.x * vLine[1].x)  +					// Ax +
					 (vNormal.y * vLine[1].y)  +					// Bx +
					 (vNormal.z * vLine[1].z)) + originDistance;	// Cz + D

		// Now that we have 2 distances from the plane, if we times them together we either
		// get a positive or negative number.  If it's a negative number, that means we collided!
		// This is because the 2 points must be on either side of the plane (IE. -1 * 1 = -1).

		if (distance1 * distance2 >= 0)			// Check to see if both point's distances are both negative or both positive
			return false;						// Return false if each point has the same sign.  -1 and 1 would mean each point is on either side of the plane.  -1 -2 or 3 4 wouldn't...

		return true;							// The line intersected the plane, Return TRUE
	}

	float PlaneDistance(CVector3f &Normal, CVector3f &Point)
	{
		float distance = 0;	// This variable holds the distance from the plane tot he origin

		// Use the plane equation to find the distance (Ax + By + Cz + D = 0)  We want to find D.
		// So, we come up with D = -(Ax + By + Cz)
		// Basically, the negated dot product of the normal of the plane and the point. (More about the dot product in another tutorial)
		distance = - ((Normal.x * Point.x) + (Normal.y * Point.y) + (Normal.z * Point.z));

		return distance;									// Return the distance
	}


	void  IntersectionPoint(CVector3f &vNormal, CVector3f vLine[], float distance,CVector3f &vIntersection)
	{
		CVector3f vPoint, vLineDir;					// Variables to hold the point and the line's direction
		float Numerator = 0.0, Denominator = 0.0, dist = 0.0;

		// 1)  First we need to get the vector of our line, Then normalize it so it's a length of 1
		vLineDir = vLine[1] - vLine[0];		// Get the Vector of the line
		vLineDir.normalize();		// Normalize the lines vector


		// 2) Use the plane equation (distance = Ax + By + Cz + D) to find the
		// distance from one of our points to the plane.
		Numerator = - (vNormal.x * vLine[0].x +		// Use the plane equation with the normal and the line
					   vNormal.y * vLine[0].y +
					   vNormal.z * vLine[0].z + distance);

		// 3) If we take the dot product between our line vector and the normal of the polygon,
		Denominator = vNormal.Dot(vLineDir);
		Denominator	= vNormal.Dot(vLineDir);	// Get the dot product of the line's vector and the normal of the plane

		// Since we are using division, we need to make sure we don't get a divide by zero error
		// If we do get a 0, that means that there are INFINATE points because the the line is
		// on the plane (the normal is perpendicular to the line - (Normal.Vector = 0)).
		// In this case, we should just return any point on the line.

		if ( Denominator == 0.0f)						// Check so we don't divide by zero
		{
			vIntersection = vLine[0];						// Return an arbitrary point on the line
			return ;
		}

		dist = Numerator / Denominator;				// Divide to get the multiplying (percentage) factor

		// Now, like we said above, we times the dist by the vector, then add our arbitrary point.
		vIntersection.x = (float)(vLine[0].x + (vLineDir.x * dist));
		vIntersection.y = (float)(vLine[0].y + (vLineDir.y * dist));
		vIntersection.z = (float)(vLine[0].z + (vLineDir.z * dist));
	}

	BOOL  InsidePolygon(CVector3f &vIntersection, CVector3f Poly[])
	{
		const double MATCH_FACTOR = 0.99;		// Used to cover up the error in floating point
		double Angle = 0.0;						// Initialize the angle
		CVector3f vA, vB;						// Create temp vectors

		for (int i = 0; i < 3; i++)		// Go in a circle to each vertex and get the angle between
		{
			if (Poly[i] == vIntersection)
				return TRUE;

			vA = Poly[i] - vIntersection;			// Subtract the intersection point from the current vertex
			// Subtract the point from the next vertex
			vB = Poly[(i + 1) % 3] - vIntersection;

			Angle += AngleBetweenVectors(vA, vB);	// Find the angle between the 2 vectors and add them all up as we go along
		}

		if (Angle >= (MATCH_FACTOR * (2.0f * 3.1415926f)) )	// If the angle is greater than 2 PI, (360 degrees)
			return true;							// The point is inside of the polygon

		return false;								// If you get here, it obviously wasn't inside the polygon, so Return FALSE
	}

	float AngleBetweenVectors(CVector3f &Vector1, CVector3f &Vector2)
	{
		// Get the dot product of the vectors
		float dotProduct = Vector1.Dot(Vector2);

		// Get the product of both of the vectors magnitudes
		float vectorsMagnitude = Magnitude(Vector1) * Magnitude(Vector2) ;

		// Get the angle in radians between the 2 vectors
		float angle = acosf( dotProduct / vectorsMagnitude );

		// Here we make sure that the angle is not a -1.#IND0000000 number, which means indefinate
		if (_isnan(angle))
			return 0;

		// Return the angle in radians
		return( angle );
	}

	float Magnitude(const CVector3f &vNormal)
	{
		// This will give us the magnitude or "Norm" as some say, of our normal.
		// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)  Where V is the vector

		return (float)sqrt( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
	}

	// collision test
	BOOL LineSegmentIntesectTriangle(CVector3f vLineSegment[2],CVector3f vTriangle[3],CVector3f &vIntersection)
	{
		CVector3f vNormal;
		float originDistance = 0;

		// First, make sure our line intersects the plane
		// Reference   // Reference
		if (!IntersectedPlane(vTriangle, vLineSegment,   vNormal,   originDistance))
			return FALSE;

		// Now that we have our normal and distance passed back from IntersectedPlane(),
		// we can use it to calculate the intersection point.

		IntersectionPoint(vNormal, vLineSegment, originDistance,vIntersection);

		// Now that we have the intersection point, we need to test if it's inside the polygon.
		if (InsidePolygon(vIntersection, vTriangle))
			return TRUE;							// We collided!	  Return success

		return FALSE;								// There was no collision, so return false
	}

	// for pick
	BOOL RadialIntersectTriangle( const CVector3f& vOrigin,const CVector3f& vDirection,const CVector3f vTriangle[3],CVector3f &vIntersection,float & fDistance)
	{
		float u,v;
		// Find vectors for two edges sharing vert0
		CVector3f edge1 = vTriangle[1] - vTriangle[0];
		CVector3f edge2 = vTriangle[2] - vTriangle[0];

		// Begin calculating determinant - also used to calculate U parameter
		CVector3f pvec;
		pvec = vDirection.Cross(edge2);

		// If determinant is near zero, ray lies in plane of triangle
		FLOAT det = edge1.Dot(pvec);

		CVector3f tvec;
		if ( det > 0 )
		{
			tvec = vOrigin - vTriangle[0];
		}
		else
		{
			tvec = vTriangle[0] - vOrigin;
			det = -det;
		}

		if ( det < 0.0001f )
			return FALSE;

		// Calculate U parameter and test bounds
		u = tvec.Dot(pvec);
		if ( u < 0.0f || u > det )
			return FALSE;

		// Prepare to test V parameter
		CVector3f qvec;
		qvec = tvec.Cross(edge1);

		// Calculate V parameter and test bounds
		v = vDirection.Dot(qvec);
		if ( v < 0.0f || u + v > det )
			return FALSE;

		// Calculate t, scale parameters, ray intersects triangle
		fDistance = edge2.Dot(qvec);
		FLOAT fInvDet = 1.0f / det;
		fDistance *= fInvDet;
		u *= fInvDet;
		v *= fInvDet;

		vIntersection = vTriangle[0] + u*(vTriangle[1] - vTriangle[0]) + v *(vTriangle[2] - vTriangle[0]);

		return TRUE;
	}

	BOOL RadialIntersectedPlane(const CVector3f &vOrigin,const CVector3f &vDirection,CVector3f vTriangle[3],CVector3f &vIntersection)
	{
		CVector3f vNormal;
		float originDistance = 0;

		CVector3f vLineSegment[2];
		vLineSegment[0] = vOrigin;
		vLineSegment[1] = vOrigin + vDirection;

		// First, make sure our line intersects the plane
		// Reference   // Reference
		IntersectedPlane(vTriangle, vLineSegment,   vNormal,   originDistance);

		// Now that we have our normal and distance passed back from IntersectedPlane(),
		// we can use it to calculate the intersection point.

		IntersectionPoint(vNormal, vLineSegment, originDistance,vIntersection);

		return TRUE;
	}

	BOOL RadialIntersectedPlane(const CVector3f &vOrigin,const CVector3f &vDirection,const CPlane &Plane,CVector3f &vIntersection)
	{
		float distance1=0, distance2=0;	// The distances from the 2 points of the line from the plane

		CVector3f vNormal(Plane.m_Normal);

		float Numerator = 0.0, Denominator = 0.0, dist = 0.0;

		// 2) Use the plane equation (distance = Ax + By + Cz + D) to find the distance from one of our points to the plane.
		Numerator = - (vNormal.x * vOrigin.x +		// Use the plane equation with the normal and the line
					   vNormal.y * vOrigin.y +
					   vNormal.z * vOrigin.z + Plane.m_Dist);

		// 3) If we take the dot product between our line vector and the normal of the polygon,
		Denominator = vNormal.Dot(vDirection);		// Get the dot product of the line's vector and the normal of the plane

		// Since we are using division, we need to make sure we don't get a divide by zero error
		// If we do get a 0, that means that there are INFINATE points because the the line is
		// on the plane (the normal is perpendicular to the line - (Normal.Vector = 0)).
		// In this case, we should just return any point on the line.

		if ( Denominator == 0.0f)						// Check so we don't divide by zero
		{
			vIntersection = vOrigin;						// Return an arbitrary point on the line
			return TRUE;
		}

		dist = Numerator / Denominator;				// Divide to get the multiplying (percentage) factor

		// Now, like we said above, we times the dist by the vector, then add our arbitrary point.
		vIntersection.x = (float)(vOrigin.x + (vDirection.x * dist));
		vIntersection.y = (float)(vOrigin.y + (vDirection.y * dist));
		vIntersection.z = (float)(vOrigin.z + (vDirection.z * dist));
		return TRUE;
	}


	bool SpherePolygonCollision(CVector3f vPolygon[],CVector3f &vCenter,float radius,CVector3f &vIntersection)
	{
		CVector3f vNormal;
		CVector3f vA = vPolygon[2] - vPolygon[0];
		CVector3f vB = vPolygon[1] - vPolygon[0];

		vNormal = vA.Cross(vB);		// Take the cross product of our 2 vectors to get a perpendicular vector
		vNormal.normalize();

		// This will store the distance our sphere is from the plane
		float distance = 0.0f;

		if (ClassifySphere(vCenter, vNormal, vPolygon[0], radius, distance))
		{
			CVector3f vOffset = vNormal * distance;

			// Once we have the offset to the plane, we just subtract it from the center
			// of the sphere.  "vPosition" now a point that lies on the plane of the polygon.
			// Whether it is inside the polygon's perimeter is another story.  Usually it
			// is though, unless we get near the edges of the polygon.
			vIntersection = vCenter - vOffset;

			// 3) STEP THREE - Check if the intersection point is inside the polygons perimeter

			// This is the same function used in our previous tutorial on Ray to Polygon Collision.
			// If the intersection point is inside the perimeter of the polygon, it returns true.
			// We pass in the intersection point, the list of vertices and vertex count of the poly.
			if (InsidePolygon(vIntersection, vPolygon) ||
					EdgeSphereCollision(vCenter, vPolygon, 3, radius))
			{
				return TRUE;	// We collided! "And you doubted me..." - Sphere
			}

		}

		// If we get here, there is obviously no collision happening up in this crib
		return false;
	}

	BOOL ClassifySphere(CVector3f &vCenter,CVector3f &vNormal, CVector3f &vPoint, float radius, float &distance)
	{
		float d = PlaneDistance(vNormal, vPoint);

		distance = (vNormal.x * vCenter.x + vNormal.y * vCenter.y + vNormal.z * vCenter.z + d);

		if (fabs(distance) < radius)
			return TRUE;
		return FALSE;
	}

	bool EdgeSphereCollision(CVector3f &vCenter,
							 CVector3f vPolygon[], int vertexCount, float radius)
	{
		CVector3f vPoint;

		// This function takes in the sphere's center, the polygon's vertices, the vertex count
		// and the radius of the sphere.  We will return true from this function if the sphere
		// is intersecting any of the edges of the polygon.  How it works is, every edge line
		// segment finds the closest point on that line to the center of the sphere.  If the
		// distance from that closest point is less than the radius of the sphere, there was
		// a collision.  Otherwise, we are definately out of reach of the polygon.  This works
		// for triangles, quads, and any other convex polygons.

		// Go through all of the vertices in the polygon
		for (int i = 0; i < vertexCount; i++)
		{
			// This returns the closest point on the current edge to the center of the sphere.
			// Notice that we mod the second point of the edge by our vertex count.  This is
			// so that when we get to the last edge of the polygon, the second vertex of the
			// edge is the first vertex that we starting with.
			ClosestPointOnLine(vPolygon[i], vPolygon[(i + 1) % vertexCount], vCenter,vPoint);

			// Now, we want to calculate the distance between the closest point and the center
			float distance = Distance(vPoint, vCenter);

			// If the distance is less than the radius, there must be a collision so return true
			if (distance < radius)
				return true;
		}

		// The was no intersection of the sphere and the edges of the polygon
		return false;
	}

	void ClosestPointOnLine(const CVector3f &vA, const CVector3f &vB, const CVector3f &vPoint,CVector3f &vClosestPoint)
	{
		// Create the vector from end point vA to our point vPoint.
		CVector3f vVector1 = vPoint - vA;

		// Create a normalized direction vector from end point vA to end point vB
		CVector3f vVector2 = vB - vA;
		vVector2.normalize();

		// Use the distance formula to find the distance of the line segment (or magnitude)
		float d = Distance(vA, vB);

		// Using the dot product, we project the vVector1 onto the vector vVector2.
		// This essentially gives us the distance from our projected vector from vA.
		float t = vVector2.Dot(vVector1);

		// If our projected distance from vA, "t", is less than or equal to 0, it must
		// be closest to the end point vA.  We want to return this end point.
		if (t <= 0)
		{
			vClosestPoint = vA;
			return ;
		}

		// If our projected distance from vA, "t", is greater than or equal to the magnitude
		// or distance of the line segment, it must be closest to the end point vB.  So, return vB.
		if (t >= d)
		{
			vClosestPoint = vB;
			return ;
		}

		// Here we create a vector that is of length t and in the direction of vVector2
		CVector3f vVector3 = vVector2 * t;

		// To find the closest point on the line segment, we just add vVector3 to the original
		// end point vA.
		vClosestPoint = vA + vVector3;

		// Return the closest point on the line segment

	}

	float Distance(const CVector3f &vPoint1, const CVector3f &vPoint2)
	{
		float distance = sqrtf( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
								(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
								(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

		return distance;
	}

	float GetRandomFloat(float fLowBound, float fHighBound)
	{
		if ( fLowBound >= fHighBound )
			return fLowBound;

		float f = (rand() % 10000) * 0.0001f;

		return (f * (fHighBound - fLowBound)) + fLowBound;
	}

	int GetRandomInt(int nLowBound, int nHighBound)
	{
		if ( nLowBound >= nHighBound )
			return nLowBound;

		int n = nHighBound - nLowBound + 1;

		return nLowBound + rand() % n;
	}
}