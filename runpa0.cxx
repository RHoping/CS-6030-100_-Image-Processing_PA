#include "pa0.cxx"
// --- Main: quick test ---
int main() {
    // === PA0_1a: Vec3 ===
    std::cout << "=== PA0_1a: Vec3 ===\n";
    Vec3 v(1,2,3), n=Vec3(0,1,0).unit(), iv=Vec3(1,-1,0).unit();
    std::cout<<"unit:        "; v.unit().print();                       std::cout<<"\n";
    std::cout<<"rand disk:   "; Vec3::random_in_unit_disk().print();    std::cout<<"\n";
    std::cout<<"rand sphere: "; Vec3::random_in_unit_sphere().print();  std::cout<<"\n";
    std::cout<<"rand unit:   "; Vec3::random_unit_vector().print();     std::cout<<"\n";
    std::cout<<"hemisphere:  "; Vec3::random_on_hemisphere(n).print();  std::cout<<"\n";
    std::cout<<"reflect:     "; Vec3::reflect(iv,n).print();            std::cout<<"\n";
    std::cout<<"refract:     "; Vec3::refract(iv,n,1.0/1.5).print();   std::cout<<"\n\n";
    // === PA0_1b: Point3 ===
    std::cout << "=== PA0_1b: Point3 ===\n";
    Point3 p1(1,2,3), p2(4,-1,2), ip=Point3(1,-1,0).unit(), np=Point3(0,1,0).unit();
    std::cout<<"add:     "; (p1+p2).print();                            std::cout<<"\n";
    std::cout<<"sub:     "; (p1-p2).print();                            std::cout<<"\n";
    std::cout<<"mul:     "; (p1*2).print();                             std::cout<<"\n";
    std::cout<<"div:     "; (p1/2).print();                             std::cout<<"\n";
    std::cout<<"length:  " << Point3(3,0,4).length()                   <<"\n";
    std::cout<<"unit:    "; Point3(3,0,4).unit().print();               std::cout<<"\n";
    std::cout<<"reflect: "; Point3::reflect(ip,np).print();             std::cout<<"\n";
    std::cout<<"refract: "; Point3::refract(ip,np,1.0/1.5).print();    std::cout<<"\n\n";
    // === PA0_1c: Color ===
    std::cout << "=== PA0_1c: Color ===\n";
    Color c1(0.5, 0.2, 0.8), c2(0.1, 0.3, 0.1);
    std::cout<<"c1:        "; c1.writeColor();
    std::cout<<"c1 + c2:   "; (c1+c2).writeColor();
    std::cout<<"c1 * 0.5:  "; (c1*0.5).writeColor();
    std::cout<<"c1 * c2:   "; (c1*c2).writeColor();
    std::cout<<"\n";
    // === PA0_1d: Ray ===
    std::cout << "=== PA0_1d: Ray ===\n";
    Ray ray(Point3(0,0,0), Vec3(1,2,3));
    std::cout<<"at t=0:  "; ray.at(0).print(); std::cout<<"\n";
    std::cout<<"at t=1:  "; ray.at(1).print(); std::cout<<"\n";
    std::cout<<"at t=2:  "; ray.at(2).print(); std::cout<<"\n\n";
    // === PA0_1e: Interval ===
    std::cout << "=== PA0_1e: Interval ===\n";
    Interval i(1.0, 5.0);
    std::cout<<"contains 3:  "<<std::boolalpha<<i.contains(3) <<"\n";
    std::cout<<"surrounds 1: "               <<i.surrounds(1) <<"\n";
    std::cout<<"size:        "               <<i.size()       <<"\n";
    i.expand(2.0);
    std::cout<<"after expand — min: "<<i.min<<"  max: "<<i.max           <<"\n";
    std::cout<<"empty size:  "       <<Interval::empty().size()           <<"\n";
    std::cout<<"universe has 1000: " <<Interval::universe().contains(1000)<<"\n\n";
    // === PA0_2: HitRecord, Hittable, Sphere, HittableList===
    std::cout << "=== PA0_2: Sphere + HittableList ===\n";
    Ray      ray2(Point3(0,0,0), Vec3(0,0,1)); // Ray pointing in +z
    Interval interval(0.001, 1e9);             // Valid t range (ignore hits behind camera)
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3(0,0,3), 1.0)); // Closer sphere
    world.add(std::make_shared<Sphere>(Point3(0,0,6), 1.0)); // Farther sphere

    HitRecord rec;
    bool hit = world.hit(ray2, interval, rec);

    std::cout<<"Hit anything?: "<<std::boolalpha<<hit        <<"\n";
    if (hit) {
        std::cout<<"Closest t:     "            <<rec.t          <<"\n";
        std::cout<<"Hit point:     "; rec.p.print();              std::cout<<"\n";
        std::cout<<"Normal:        "; rec.normal.print();         std::cout<<"\n";
        std::cout<<"Front face?:   "            <<rec.front_face <<"\n";
    }

    return 0;
}