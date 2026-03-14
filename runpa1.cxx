#include "pa1.cxx"
//Main: quick test
int main() {
    std::cout << "=== PA2 Q1a: Material (abstract) ===\n";
    Ray      ray(Point3(0,0,0), Vec3(0,0,1));
    Interval interval(0.001, 1e9);
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3(0,0,3), 1.0));
    HitRecord rec;
    std::cout << "Sphere hit test: " << std::boolalpha << world.hit(ray, interval, rec) << "\n";

    std::cout << "=== PA2 Q1b: Lambertian ===\n";
    Lambertian mat(Color(0.8, 0.3, 0.3));         // Red-ish Lambertian material
    std::cout << "albedo: "; mat.albedo.writeColor();

    // Simulate a hit and scatter
    Ray      ray_in(Point3(0,0,0), Vec3(0,0,1));
    //HitRecord rec;
    rec.p      = Point3(0, 0, 2);                 // Hit point
    rec.normal = Vec3(0, 0, -1);                  // Normal pointing back at camera
    rec.front_face = true;

    Color attenuation;
    Ray   scattered;
    bool  did_scatter = mat.scatter(ray_in, rec, attenuation, scattered);

    std::cout << "scattered?:  " << std::boolalpha << did_scatter       << "\n";
    std::cout << "attenuation: "; attenuation.writeColor();
    std::cout << "scatter dir: "; scattered.direction.print();   std::cout << "\n";

    std::cout << "=== PA2 Q1c: Metal ===\n";
    Metal metal(Color(0.8, 0.8, 0.2), 0.3);
    std::cout << "albedo: "; metal.albedo.writeColor();
    std::cout << "fuzz:   " << metal.fuzz << "\n";

    std::cout << "=== PA2 Q1d: Dielectric ===\n";
    Dielectric glass(1.5);                     // Glass with ior = 1.5
    std::cout << "ior: " << glass.ior << "\n";

    std::cout << "=== PA2 Q2b: Camera initialize() ===\n";
    Camera cam;
    cam.initialize();
    std::cout << "image_width:  " << cam.image_width  << "\n";
    std::cout << "image_height: " << cam.image_height << "\n";
    std::cout << "initialized successfully\n";

    std::cout << "=== PA2 Q2c: get_ray() ===\n";
    cam.initialize();
    Ray r = cam.get_ray(200, 112);           // Sample center pixel
    std::cout << "ray origin: ";    r.origin.print();    std::cout << "\n";
    std::cout << "ray direction: "; r.direction.print(); std::cout << "\n";

    std::cout << "=== PA2 Q2d: pixel_sample_square() ===\n";
    // Call via get_ray which uses pixel_sample_square internally
    Ray r2 = cam.get_ray(0, 0);
    std::cout << "sample ray origin:    "; r2.origin.print();    std::cout << "\n";
    std::cout << "sample ray direction: "; r2.direction.print(); std::cout << "\n";

    std::cout << "=== PA2 Q2e: defocus_disk_sample() ===\n";
    Camera cam2;
    cam2.defocus_angle = 10.0;             // Set non-zero so disk sample differs from center
    cam2.initialize();
    Ray r3 = cam2.get_ray(200, 112);
    std::cout << "ray origin (on disk): "; r3.origin.print(); std::cout << "\n";

    std::cout << "=== PA2 Q2f: ray_color() ===\n";
    Ray   test_ray(Point3(0,0,0), Vec3(0,1,0));  // Ray pointing up — hits background
    Color c = cam.ray_color(test_ray, world, 50);
    std::cout << "sky color: "; c.writeColor();

    std::cout << "=== PA2 Q2g: render() ===\n";
    HittableList scene;
    scene.add(std::make_shared<Sphere>(Point3(0,0,-1), 0.5));
    scene.add(std::make_shared<Sphere>(Point3(0,-100.5,-1), 100));
    cam.render(scene);
    std::cout << "Image written to output.ppm\n";
    std::system("magick output.ppm output.png");
    return 0;
}