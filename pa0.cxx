#include <cmath>
#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <limits>

//Utility
inline double random_double(double min = 0.0, double max = 1.0) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_real_distribution<double>(min, max)(gen);
}

//PA0_1a Vec3
class Vec3 {
public:
    double x, y, z;
    Vec3() : x(0), y(0), z(0) {}                               // Default constructor
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}  // Parameterized constructor

    Vec3 operator-()              const { return Vec3(-x,-y,-z); }                    // Unary negation
    Vec3 operator+(const Vec3& o) const { return Vec3(x+o.x, y+o.y, z+o.z); }        // Addition
    Vec3 operator-(const Vec3& o) const { return Vec3(x-o.x, y-o.y, z-o.z); }        // Subtraction
    Vec3 operator*(const Vec3& o) const { return Vec3(x*o.x, y*o.y, z*o.z); }        // Component-wise multiply
    Vec3 operator*(double t)      const { return Vec3(x*t, y*t, z*t); }               // Scalar multiply
    Vec3 operator/(double t)      const { return *this*(1.0/t); }                     // Scalar divide
    Vec3& operator+=(const Vec3& o)     { x+=o.x; y+=o.y; z+=o.z; return *this; }    // += assignment
    Vec3& operator*=(double t)          { x*=t; y*=t; z*=t; return *this; }           // *= assignment

    double length_squared()          const { return x*x+y*y+z*z; }                                   // Squared magnitude
    double length()                  const { return std::sqrt(length_squared()); }                    // Magnitude
    double dot(const Vec3& o)        const { return x*o.x+y*o.y+z*o.z; }                            // Dot product
    Vec3   cross(const Vec3& o)      const { return Vec3(y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x); } // Cross product
    Vec3   unit()                    const { double L=length(); return L?*this/L:Vec3(); }            // Unit vector
    bool   near_zero(double e=1e-12) const { return std::abs(x)<e&&std::abs(y)<e&&std::abs(z)<e; }   // Near zero check

    static Vec3 random(double a=0, double b=1) { return Vec3(random_double(a,b),random_double(a,b),random_double(a,b)); } // Random vector
    static Vec3 random_in_unit_disk()   { while(true){ Vec3 p(random_double(-1,1),random_double(-1,1),0); if(p.length_squared()<1) return p; } } // Random in disk
    static Vec3 random_in_unit_sphere() { while(true){ Vec3 p=random(-1,1); if(p.length_squared()<1) return p; } }       // Random in sphere
    static Vec3 random_unit_vector()                  { return random_in_unit_sphere().unit(); }                          // Random unit vector
    static Vec3 random_on_hemisphere(const Vec3& n)   { Vec3 v=random_unit_vector(); return v.dot(n)>0?v:-v; }            // Random on hemisphere
    static Vec3 reflect(const Vec3& v, const Vec3& n) { return v-n*(2.0*v.dot(n)); }                                     // Reflection
    static Vec3 refract(const Vec3& uv, const Vec3& n, double eta) {                                                      // Refraction (Snell's Law)
        double cos_t=std::min((-uv).dot(n),1.0);
        Vec3 perp=(uv+n*cos_t)*eta, paral=n*(-std::sqrt(std::max(0.0,1.0-perp.length_squared())));
        return perp+paral;
    }
    void print() const { std::cout<<"Vec3("<<x<<","<<y<<","<<z<<")"; } // Print
};
inline Vec3 operator*(double t, const Vec3& v) { return v*t; } // Scalar * Vec3

//PA0_1b Point3
class Point3 {
public:
    double x, y, z;
    Point3() : x(0), y(0), z(0) {}                               // Default constructor
    Point3(double x, double y, double z) : x(x), y(y), z(z) {}  // Parameterized constructor

    Point3 operator-()                const { return Point3(-x,-y,-z); }                     // Unary negation
    Point3 operator+(const Point3& o) const { return Point3(x+o.x, y+o.y, z+o.z); }         // Addition
    Point3 operator+(const Vec3& o)   const { return Point3(x+o.x, y+o.y, z+o.z); }         // Point + Vec3
    Vec3   operator-(const Point3& o) const { return Vec3(x-o.x,   y-o.y,   z-o.z); }       // Point3 - Point3 => Vec3
    Vec3   operator-(const Vec3& o)   const { return Vec3(x-o.x,   y-o.y,   z-o.z); }       // Point3 - Vec3 => Vec3
    Point3 operator*(double t)        const { return Point3(x*t, y*t, z*t); }                // Scalar multiply
    Point3 operator/(double t)        const { return *this*(1.0/t); }                        // Scalar divide
    Point3& operator+=(const Vec3& o)       { x+=o.x; y+=o.y; z+=o.z; return *this; }       // += Vec3

    double length_squared()           const { return x*x+y*y+z*z; }                          // Squared magnitude
    double length()                   const { return std::sqrt(length_squared()); }           // Magnitude
    double dot(const Vec3& o)         const { return x*o.x+y*o.y+z*o.z; }                   // Dot product with Vec3
    Point3 unit()                     const { double L=length(); return L?*this/L:Point3(); } // Unit vector
    Vec3   toVec3()                   const { return Vec3(x, y, z); }                        // Convert to Vec3

    static Point3 reflect(const Point3& v, const Point3& n) {                                // Reflection
        Vec3 vv=v.toVec3(), nv=n.toVec3();
        Vec3 r = Vec3::reflect(vv, nv);
        return Point3(r.x, r.y, r.z);
    }
    static Point3 refract(const Point3& uv, const Point3& n, double eta) {                   // Refraction (Snell's Law)
        Vec3 uvv=uv.toVec3(), nv=n.toVec3();
        Vec3 r = Vec3::refract(uvv, nv, eta);
        return Point3(r.x, r.y, r.z);
    }
    void print() const { std::cout<<"Point3("<<x<<","<<y<<","<<z<<")"; } // Print
};
inline Point3 operator*(double t, const Point3& p) { return p*t; } // Scalar * Point3

//PA0_1c Color 
class Color {
public:
    double r, g, b;
    Color() : r(0), g(0), b(0) {}                               // Default constructor
    Color(double r, double g, double b) : r(r), g(g), b(b) {}  // Parameterized constructor

    Color  operator+(const Color& o) const { return Color(r+o.r, g+o.g, b+o.b); }    // Addition
    Color  operator-(const Color& o) const { return Color(r-o.r, g-o.g, b-o.b); }    // Subtraction
    Color  operator*(const Color& o) const { return Color(r*o.r, g*o.g, b*o.b); }    // Component-wise multiply
    Color  operator*(double t)       const { return Color(r*t, g*t, b*t); }           // Scalar multiply
    Color  operator/(double t)       const { return Color(r/t, g/t, b/t); }           // Scalar divide
    Color& operator+=(const Color& o)      { r+=o.r; g+=o.g; b+=o.b; return *this; } // += assignment

    void writeColor() const { // Write RGB to stdout as [0,255] for PPM
        auto clamp=[](double v){ return std::max(0.0,std::min(1.0,v)); };
        std::cout<<int(255.999*clamp(r))<<" "<<int(255.999*clamp(g))<<" "<<int(255.999*clamp(b))<<"\n";
    }
};
inline Color operator*(double t, const Color& c) { return c*t; } // Scalar * Color

//PA0_1d Ray 
class Ray {
public:
    Point3 origin;     // Starting point
    Vec3   direction;  // Direction vector
    Ray() {}                                                          // Default constructor
    Ray(const Point3& o, const Vec3& d) : origin(o), direction(d) {} // Parameterized constructor

    Point3 at(double t) const { // P(t) = origin + t * direction
        return Point3(origin.x+direction.x*t, origin.y+direction.y*t, origin.z+direction.z*t);
    }
};

//PA0_1e Interval
class Interval {
public:
    double min, max;
    Interval() : min(std::numeric_limits<double>::infinity()), max(-std::numeric_limits<double>::infinity()) {} // Default: empty
    Interval(double mn, double mx) : min(mn), max(mx) {}  // Parameterized constructor

    bool   contains(double x)  const { return min<=x && x<=max; } // Inclusive check
    bool   surrounds(double x) const { return min<x  && x<max;  } // Exclusive check (avoids self-intersection)
    double size()              const { return max-min; }           // Width of interval
    void   expand(double d)          { min-=d/2.0; max+=d/2.0; }  // Expand by delta

    static Interval empty()    { return Interval( std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()); } // Empty interval
    static Interval universe() { return Interval(-std::numeric_limits<double>::infinity(),  std::numeric_limits<double>::infinity()); } // All reals
};

//PA0_2a HitRecord
struct HitRecord {
    Point3 p;                  // Hit point in 3D
    Vec3   normal;             // Surface normal (oriented to oppose ray)
    double t          = 0.0;   // Ray parameter at hit
    bool   front_face = false; // True if ray hits outside surface

    void set_face_normal(const Ray& r, const Vec3& outward_normal) { // Orient normal against ray
        front_face = r.direction.dot(outward_normal) < 0;            // Opposite => front face
        normal     = front_face ? outward_normal : -outward_normal;  // Always opposes ray
    }
};

//PA0_2b Abstract Hittable 
class Hittable {
public:
    virtual ~Hittable() = default;                                                        // Virtual destructor
    // PA0_2c Returns (hit_anything: bool, record: HitRecord)
    //         If no hit, returns (false, default HitRecord)
    virtual bool hit(const Ray& r, const Interval& interval, HitRecord& rec) const = 0; // Pure virtual — returns bool, fills rec via ref
};

//PA0_2d Sphere
class Sphere : public Hittable {
public:
    Point3 center;  // Sphere center
    double radius;  // Sphere radius
    Sphere(const Point3& c, double r) : center(c), radius(r) {} // Parameterized constructor

    bool hit(const Ray& r, const Interval& interval, HitRecord& rec) const override {
        Vec3   oc           = r.origin - center;                    // O - C
        double a            = r.direction.length_squared();         // D·D
        double half_b       = oc.dot(r.direction);                  // (O-C)·D
        double c            = oc.length_squared() - radius*radius;  // |O-C|^2 - R^2
        double discriminant = half_b*half_b - a*c;                  // Discriminant

        if (discriminant < 0) return false;                         // No real roots => no intersection

        double sqrt_d = std::sqrt(discriminant);                    // sqrt(discriminant) computed once
        double root   = (-half_b - sqrt_d) / a;                    // Try smaller root first (closest hit)
        if (!interval.surrounds(root)) {
            root = (-half_b + sqrt_d) / a;                          // Try larger root
            if (!interval.surrounds(root)) return false;            // Both invalid => no hit
        }

        rec.t = root;                                               // Store hit parameter
        rec.p = r.at(rec.t);                                        // Store hit point
        Vec3 outward_normal = (rec.p - center) / radius;           // Outward normal (not yet oriented)
        rec.set_face_normal(r, outward_normal);                     // Orient normal to always face against ray

        return true;                                                // bool returned, rec filled via ref
    }
};

// --- PA0_2e HittableList ---
class HittableList : public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objects;                   // Container for all hittable objects

    void add(std::shared_ptr<Hittable> obj) { objects.push_back(obj); } // Add a hittable object
    void clear()                            { objects.clear(); }         // Remove all objects

    bool hit(const Ray& r, const Interval& interval, HitRecord& rec) const override {
        bool   hit_anything   = false;         // Track whether we've hit anything so far
        double closest_so_far = interval.max;  // Track closest t; start with interval.max

        for (const auto& obj : objects) {
            HitRecord temp_rec;
            if (obj->hit(r, Interval(interval.min, closest_so_far), temp_rec)) { // Restrict far end to closest
                hit_anything   = true;         // Hit at least one object
                closest_so_far = temp_rec.t;   // Update closest distance
                rec            = temp_rec;     // Store closest record
            }
        }
        return hit_anything;  
    }

};
