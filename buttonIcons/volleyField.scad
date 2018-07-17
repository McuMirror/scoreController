union() {
    // Fondo
    color([0.85, 0.85, 1.0])
        cube([2800, 1900, 2.9], center=true);
    
    // Campo di gioco
    color("yellow")
        cube([1800, 900, 3], center=true);
    
    // Linee
    color("white") {
        cube([40, 900, 3.1], center = true);
        translate([0,-445, 0])
            cube([1800, 40, 3.1], center=true);
        translate([0, 445, 0])
            cube([1800, 40, 3.1], center=true);
        translate([0, 445, 0])
            cube([1800, 40, 3.1], center=true);
           
        translate([295, 0, 0])
            cube([40, 900, 3.1], center = true);
        translate([-295, 0, 0])
            cube([40, 900, 3.1], center = true);
           
        translate([895, 0, 0])
            cube([40, 900, 3.1], center = true);
        translate([-895, 0, 0])
            cube([40, 900, 3.1], center = true);
    }
    // Pali
    color("orange") {
        translate([0,-545, 0])
            cylinder(d=40, h=250);
        translate([0, 545, 0])
            cylinder(d=40, h=250);
    }
    
    // Rete
    color("silver") {// Bande
        translate([0, 0, 225])
            cube([1, 1090, 5], center=true);
        translate([0, 0, 125])
            cube([1, 1090, 5], center=true);
         translate([0, 445, 175])
            cube([1, 5, 100], center=true);
         translate([0,-445, 175])
            cube([1, 5, 100], center=true);
  }
  color("red") {
      for(y=[20:40:860]) {
          translate([0, y-440, 175])
            cylinder(d=4, h=100, center=true);
      }
      for(z=[20:20:80]) {
            translate([0, 0, 120+z])
                rotate([90, 0, 0])
                    cylinder(d=4, h=885, center=true);
      }
  }
}