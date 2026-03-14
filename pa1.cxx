#include "pa0.cxx" // Include PA0 code for Vec3, Point3, Color, Ray, Interval, HitRecord, Hittable, Sphere, HittableList
#include <fstream>
#include <cmath>
//PA2_1a Abstract Material 
class Material {
public:
    virtual ~Material() = default;  // Virtual destructor

    virtual bool scatter(
        const Ray&       ray_in,      // Incoming ray
        const HitRecord& rec,         // Hit record at intersection point
        Color&           attenuation, // Output: color/attenuation (via ref)
        Ray&             scattered    // Output: scattered ray (via ref)
    ) const = 0;                      // Pure virtual — must be implemented by subclass
};

//PA2_1b Lambertian Material 
class Lambertian : public Material {
public:
    Color albedo;  // Single color field — the diffuse color of the surface

    Lambertian(const Color& a) : albedo(a) {} // Parameterized constructor

    bool scatter(
        const Ray&       ray_in,      // Incoming ray (unused for Lambertian)
        const HitRecord& rec,         // Hit record (provides normal + hit point)
        Color&           attenuation, // Output: surface color
        Ray&             scattered    // Output: scattered ray in random diffuse direction
    ) const override {
        Vec3 scatter_dir = rec.normal + Vec3::random_unit_vector(); // Lambertian diffuse direction

        if (scatter_dir.near_zero())      // If scatter direction is near zero
            scatter_dir = rec.normal;     // Fall back to normal to avoid degenerate ray

        scattered    = Ray(rec.p, scatter_dir); // Scattered ray from hit point
        attenuation  = albedo;                  // Output the surface color
        return true;                            // Lambertian always scatters
    }
};

//PA2_1c Metal Material
class Metal : public Material {
public:
    Color  albedo;  // Color of the metal
    double fuzz;    // Fuzziness/sharpness of reflection (0 = perfect mirror, 1 = very fuzzy)
 
    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {} // Clamp fuzz to [0,1]
 
    bool scatter(
        const Ray&       ray_in,      // Incoming ray
        const HitRecord& rec,         // Hit record (provides normal + hit point)
        Color&           attenuation, // Output: metal color
        Ray&             scattered    // Output: reflected ray + fuzz perturbation
    ) const override {
        Vec3 reflected = Vec3::reflect(ray_in.direction.unit(), rec.normal); // Perfect reflection
        scattered   = Ray(rec.p, reflected + Vec3::random_unit_vector() * fuzz); // Add fuzz perturbation
        attenuation = albedo;                                                     // Output metal color
        return scattered.direction.dot(rec.normal) > 0;  // Only scatter if reflected above surface
    }
};

//PA2_1d Dielectric Material
class Dielectric : public Material {
public:
    double ior;  // Index of refraction (e.g. 1.0=air, 1.5=glass, 2.4=diamond)

    Dielectric(double index) : ior(index) {} // Parameterized constructor

    bool scatter(
        const Ray&       ray_in,      // Incoming ray
        const HitRecord& rec,         // Hit record (provides normal + hit point)
        Color&           attenuation, // Output: always white (glass doesn't absorb)
        Ray&             scattered    // Output: refracted or reflected ray
    ) const override {
        attenuation        = Color(1.0, 1.0, 1.0);              // Glass absorbs nothing
        double ratio       = rec.front_face ? (1.0/ior) : ior;  // Air->glass or glass->air
        Vec3   unit_dir    = ray_in.direction.unit();            // Normalize incoming direction

        double cos_theta   = std::min((-unit_dir).dot(rec.normal), 1.0); // cos of incident angle
        double sin_theta   = std::sqrt(1.0 - cos_theta*cos_theta);       // sin of incident angle

        // Total internal reflection: if sin_theta * ratio > 1, can't refract
        bool cannot_refract = ratio * sin_theta > 1.0;
        Vec3 direction;
        if (cannot_refract || reflectance(cos_theta, ratio) > random_double())
            direction = Vec3::reflect(unit_dir, rec.normal);              // Reflect
        else
            direction = Vec3::refract(unit_dir, rec.normal, ratio);       // Refract (Snell's Law)

        scattered = Ray(rec.p, direction);                                // Output scattered ray
        return true;                                                      // Dielectric always scatters
    }

private:
    // Schlick's approximation for reflectance
    static double reflectance(double cosine, double ref_idx) {
        double r0 = (1 - ref_idx) / (1 + ref_idx);  // Base reflectance at normal incidence
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow(1 - cosine, 5); // Schlick approximation
    }
};

//PA2_2a Camera
class Camera {
public:
    //Image settings
    double aspect_ratio      = 16.0 / 9.0;  // i.   Ratio of image width to height
    int    image_width       = 400;          // ii.  Rendered image width in pixels
    int    image_height      = 225;          // iii. Rendered image height in pixels
    int    samples_per_pixel = 10;           // iv.  Rays per pixel for anti-aliasing
    int    max_depth         = 50;           // v.   Maximum ray bounce depth

    //Camera orientation
    double vfov     = 90.0;                  // vi.  Vertical field of view (degrees)
    Point3 lookfrom = Point3(0, 0, 0);       // vii. Camera position in world space
    Point3 lookat   = Point3(0, 0, -1);      // viii.Point the camera is aimed at
    Vec3   vup      = Vec3(0, 1, 0);         // ix.  Camera-relative "up" direction

    //Depth of field
    double focus_dist   = 10.0;              // x.   Distance to plane of perfect focus
    double defocus_angle = 0.0;              // xi.  Cone angle of defocus blur (0 = no blur)


//PA2_2b Initialize
void initialize() {
    // Compute image height from width and aspect ratio (at least 1)
    image_height = std::max(1, static_cast<int>(image_width / aspect_ratio)); // iii.

    center = lookfrom;                   // xii. Camera center = lookfrom point

    // Compute viewport dimensions from vfov and focus distance
    double theta      = vfov * M_PI / 180.0;          // Convert vfov degrees to radians
    double h          = std::tan(theta / 2.0);         // Half-height at unit distance
    double vp_height  = 2.0 * h * focus_dist;          // Viewport height scaled by focus dist
    double vp_width   = vp_height * (static_cast<double>(image_width) / image_height); // Viewport width

    // Build camera frame basis vectors from lookfrom, lookat, vup
    w = (lookfrom - lookat).unit();                    // xvi. w: points opposite view direction
    u = vup.cross(w).unit();                           // xvi. u: points right
    v = w.cross(u);                                    // xvi. v: points up

    // Compute viewport edge vectors along u and v
    Vec3 vp_u = u * vp_width;                          // Vector across viewport horizontally
    Vec3 vp_v = (-v) * vp_height;                      // Vector down viewport vertically

    // Compute per-pixel delta vectors
    pixel_delta_u = vp_u / image_width;                // xiv. Horizontal pixel step
    pixel_delta_v = vp_v / image_height;               // xv.  Vertical pixel step

    // Compute location of upper-left pixel (0,0)
    Point3 vp_upper_left = center
        + Point3((-w * focus_dist).x, (-w * focus_dist).y, (-w * focus_dist).z)
        + Point3((-vp_u / 2).x, (-vp_u / 2).y, (-vp_u / 2).z)
        + Point3((-vp_v / 2).x, (-vp_v / 2).y, (-vp_v / 2).z);

    pixel00_loc = vp_upper_left                        // xiii. Top-left pixel location
        + Point3((pixel_delta_u / 2 + pixel_delta_v / 2).x,
                 (pixel_delta_u / 2 + pixel_delta_v / 2).y,
                 (pixel_delta_u / 2 + pixel_delta_v / 2).z);

    // Compute defocus disk basis vectors
    double defocus_radius = focus_dist * std::tan(defocus_angle / 2.0 * M_PI / 180.0);
    defocus_disk_u = u * defocus_radius;               // xvii. Defocus disk horizontal radius
    defocus_disk_v = v * defocus_radius;               // xviii.Defocus disk vertical radius
 }
 // PA2_2c get_ray()
Ray get_ray(int i, int j) const {                              // Takes pixel coords i, j
    Vec3 offset = pixel_sample_square();                       // Random offset within pixel square
    Point3 pixel_sample = pixel00_loc                          // Start at pixel (0,0)
        + Point3((pixel_delta_u * (i + offset.x)).x,
                 (pixel_delta_u * (i + offset.x)).y,
                 (pixel_delta_u * (i + offset.x)).z)
        + Point3((pixel_delta_v * (j + offset.y)).x,
                 (pixel_delta_v * (j + offset.y)).y,
                 (pixel_delta_v * (j + offset.y)).z);

    Point3 ray_origin    = (defocus_angle <= 0) ? center : defocus_disk_sample(); // ii. Origin from defocus disk
    Vec3   ray_direction = pixel_sample - ray_origin;          // Direction toward sampled pixel

    return Ray(ray_origin, ray_direction);                     // Return sampled ray
}

// PA2_2d pixel_sample_square()
private: 
    Vec3 pixel_sample_square() const {             // No input parameters           
        return Vec3(random_double() - 0.5,   // Random offset in [-0.5, 0.5]
        random_double() - 0.5, 
        0);
    }

// PA2_2e pixel_sample_disk()
    Point3 defocus_disk_sample() const {                       // Random point on defocus disk
        Vec3 p = Vec3::random_in_unit_disk();
        return center                               // ii. Random point on defocus disk
            + Point3((defocus_disk_u * p.x).x, (defocus_disk_u * p.x).y, (defocus_disk_u * p.x).z)
            + Point3((defocus_disk_v * p.y).x, (defocus_disk_v * p.y).y, (defocus_disk_v * p.y).z);
    }
// PA2_2f ray_color()
public:
    Color ray_color(const Ray& r, const Hittable& world, int depth) const { // i. ray, hittable, depth
            if (depth <= 0)                                        // Max depth reached
                return Color(0, 0, 0);                             // Return black (no more light)

            HitRecord rec;
            if (world.hit(r, Interval(0.001, std::numeric_limits<double>::infinity()), rec)) {
                // Diffuse approximation using normal
                Vec3 direction = rec.normal + Vec3::random_unit_vector(); // Random scatter off normal
                return ray_color(Ray(rec.p, direction), world, depth - 1) * 0.5; // Recurse, attenuate by 0.5
            }
            // Background: blue-white gradient based on ray direction
            Vec3   unit_dir = r.direction.unit();                  // Normalize ray direction
            double t        = 0.5 * (unit_dir.y + 1.0);           // Map y to [0,1]
            return Color(1,1,1) * (1.0-t) + Color(0.5,0.7,1.0) * t; // ii. Lerp white to sky blue
        
        }
// PA2_2g render()
    void render(const Hittable& world) {                       // i. Takes hittable scene
        initialize();                                          // Set up camera before rendering

        std::ofstream out("output.ppm");                       // Open output PPM file
        out << "P3\n" << image_width << " " << image_height   // PPM header
            << "\n255\n";

        for (int j = 0; j < image_height; j++) {              // Loop over rows
            std::cerr << "\rScanlines remaining: " << (image_height - j) << " " << std::flush; // Progress
            for (int i = 0; i < image_width; i++) {           // Loop over columns
                Color pixel_color(0, 0, 0);                   // Accumulate color samples
                for (int s = 0; s < samples_per_pixel; s++) { // Multi-sample for anti-aliasing
                    Ray r = get_ray(i, j);                    // Get random ray for pixel (i,j)
                    pixel_color += ray_color(r, world, max_depth); // Accumulate color
                }
                pixel_color = pixel_color / samples_per_pixel; // Average samples

                // Write pixel directly 
                auto clamp = [](double v){ return std::max(0.0, std::min(1.0, v)); };
                out << int(255.999*clamp(pixel_color.r)) << " "
                    << int(255.999*clamp(pixel_color.g)) << " "
                    << int(255.999*clamp(pixel_color.b)) << "\n";
            }
        }
        std::cerr << "\nDone.\n";
        out.close();                                           // xii. Written to file
    }
private:
    //Derived / computed fields (set by initialize())
    Point3 center;          // xii.  Camera center in world space
    Point3 pixel00_loc;     // xiii. Location of top-left pixel (0,0) in world space
    Vec3   pixel_delta_u;   // xiv.  Offset to pixel to the right
    Vec3   pixel_delta_v;   // xv.   Offset to pixel below
    Vec3   u, v, w;         // xvi.  Camera frame basis vectors (right, up, back)
    Vec3   defocus_disk_u;  // xvii. Defocus disk horizontal radius vector
    Vec3   defocus_disk_v;  // xviii.Defocus disk vertical radius vector
};
