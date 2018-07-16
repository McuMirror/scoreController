union() {
    // Fondo
    color("lightblue")
        cube([2800, 1900, 2.9], center=true);
    
    // Campo di gioco
    color("yellow")
        cube([1800, 900, 3], center=true);
    
    // Linee
    color("white") {
        cube([10, 900, 3.1], center = true);
        translate([0,-445, 0])
            cube([1800, 10, 3.1], center=true);
        translate([0, 445, 0])
            cube([1800, 10, 3.1], center=true);
        translate([0, 445, 0])
            cube([1800, 10, 3.1], center=true);
           
        translate([295, 0, 0])
            cube([10, 900, 3.1], center = true);
        translate([-295, 0, 0])
            cube([10, 900, 3.1], center = true);
           
        translate([895, 0, 0])
            cube([10, 900, 3.1], center = true);
        translate([-895, 0, 0])
            cube([10, 900, 3.1], center = true);
    }
    // Pali
    color("blue") {
        translate([0,-545, 0])
            cylinder(d=20, h=250);
        translate([0, 545, 0])
            cylinder(d=20, h=250);
    }
    
    // Rete
    color("white") {// Bande
        translate([0, 0, 225])
            cube([1, 1000, 5], center=true);
        translate([0, 0, 125])
            cube([1, 1000, 5], center=true);
         translate([0, 445, 175])
            cube([1, 5, 100], center=true);
         translate([0,-445, 175])
            cube([1, 5, 100], center=true);
  }
  color("red") {
  }
}