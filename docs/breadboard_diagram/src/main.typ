#import "@preview/fletcher:0.5.8" as fletcher: diagram, edge, node
#set page(width: auto, height: auto)

#let brown = orange.darken(50%)

#let pico_top_left = <3c>
#let pico_bottom_right = <22h>

#let breadboard_hole(..args) = node(
  ..args,
  shape: rect,
  stroke: (
    paint: rgb("#CBC9BE"),
    thickness: 10pt,
  ),
  inset: 5pt,
  outset: -15pt,
  label: [
    #rect(width: 30pt, height: 30pt, fill: luma(50%), rect(
      width: 20pt,
      height: 20pt,
      fill: black.lighten(10%),
    ))
  ],
)
#let wire(color, ..args) = edge(
  ..args,
  stroke: (
    paint: color,
    thickness: 10pt,
  ),
  layer: 1,
)
#let led(color, ..args) = edge(
  ..args,
  stroke: (
    paint: color,
    thickness: 10pt,
  ),
  layer: 1,
  marks: (
    (
      inherit: "|>",
      size: 4,
      fill: color,
      // tip-end: mark => -0.5 / calc.sin(mark.sharpness),
      tip-end: mark => -mark.size * calc.cos(mark.sharpness),
      tail-origin: mark => -mark.size * calc.cos(mark.sharpness),
      sharpness: 29deg,
      stroke: 4pt + black,
      pos: 0.5,
      draw: mark => {
        // fletcher.draw.polygon((0,0), 3)
        fletcher.draw.line(
          (180deg + mark.sharpness, mark.size),
          (0, 0),
          (180deg - mark.sharpness, mark.size),
          (180deg + mark.sharpness, mark.size),
        )
        fletcher.draw.line((92deg, mark.size / 1.9), (-92deg, mark.size / 1.9))
      },
    ),
    (
      inherit: "|>",
      size: 4,
      fill: color,
      // tip-end: mark => -0.5 / calc.sin(mark.sharpness),
      tip-end: mark => -mark.size * calc.cos(mark.sharpness),
      tail-origin: mark => -mark.size * calc.cos(mark.sharpness),
      sharpness: 29deg,
      stroke: 2pt + color,
      pos: 0.5,
      draw: mark => {
        let arrw(shift) = fletcher.draw.line(
          ((-0.7 + shift.at(0)) * mark.size, (0.6 + shift.at(1)) * mark.size),
          ((-0.6 + shift.at(0)) * mark.size, (0.8 + shift.at(1)) * mark.size),
          (
            (-0.654 + shift.at(0)) * mark.size,
            (0.83 + shift.at(1)) * mark.size,
          ),
          ((-0.56 + shift.at(0)) * mark.size, (0.87 + shift.at(1)) * mark.size),
          (
            (-0.546 + shift.at(0)) * mark.size,
            (0.77 + shift.at(1)) * mark.size,
          ),
          ((-0.6 + shift.at(0)) * mark.size, (0.8 + shift.at(1)) * mark.size),
        )
        arrw((0, 0))
        arrw((0.27, -0.14))
      },
    ),
  ),
)
#let capacitor(..args) = edge(
  ..args,
  stroke: (
    paint: brown,
    thickness: 10pt,
  ),
  layer: 1,
  marks: (
    (
      inherit: "|>",
      size: 4,
      fill: white,
      // tip-end: mark => -0.5 / calc.sin(mark.sharpness),
      tip-end: mark => -0.2 * mark.size,
      tail-end: mark => 0.2 * mark.size,
      tip-origin: mark => 0.2 * mark.size,
      tail-origin: mark => -0.2 * mark.size,
      sharpness: 29deg,
      stroke: 4pt + brown,
      pos: 0.5,
      draw: mark => {
        // fletcher.draw.polygon((0,0), 3)
        let plus_thickness = 0.09
        let plus_length = 0.15
        let plus_pos = (0.4, 0.41)

        fletcher.draw.rect(
          (0.2 * mark.size, -13pt),
          (-0.2 * mark.size, 13pt),
          stroke: white,
        )
        fletcher.draw.line((0.2 * mark.size, 0.6 * mark.size), (
          0.2 * mark.size,
          -0.6 * mark.size,
        ))
        fletcher.draw.line((-0.2 * mark.size, 0.6 * mark.size), (
          -0.2 * mark.size,
          -0.6 * mark.size,
        ))
        fletcher.draw.line(
          (
            (plus_pos.at(0) + plus_length * 0 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 0) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 0) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * -1 + plus_thickness * 0)
              * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 1) * mark.size,
            (plus_pos.at(1) + plus_length * -1 + plus_thickness * 0)
              * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 1) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 0) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 2 + plus_thickness * 1) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 0) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 2 + plus_thickness * 1) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 1) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 1) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 1) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 1) * mark.size,
            (plus_pos.at(1) + plus_length * 1 + plus_thickness * 1) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * 1 + plus_thickness * 1) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 1 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 1) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 0 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 1) * mark.size,
          ),
          (
            (plus_pos.at(0) + plus_length * 0 + plus_thickness * 0) * mark.size,
            (plus_pos.at(1) + plus_length * 0 + plus_thickness * 0) * mark.size,
          ),
          // (0.5 * mark.size, 0.5 * mark.size),
          // (0.3 * mark.size, 0.5 * mark.size),
          // (0.3 * mark.size, 0.58 * mark.size),
          // (0.5 * mark.size, 0.58 * mark.size),
          // (0.5 * mark.size, 0.78 * mark.size),
          // (0.58 * mark.size, 0.78 * mark.size),
          // (0.58 * mark.size, 0.58 * mark.size),
          // (0.78 * mark.size, 0.58 * mark.size),
          // (0.78 * mark.size, 0.5 * mark.size),
          // (0.58 * mark.size, 0.5 * mark.size),
          // (0.58 * mark.size, 0.5 * mark.size),
          fill: brown,
          stroke: 0.5pt + white,
          // radius: 2pt,
        )


        // fletcher.draw.line(
        //   (180deg + mark.sharpness, mark.size),
        //   (0, 0),
        //   (180deg - mark.sharpness, mark.size),
        //   (180deg + mark.sharpness, mark.size),
        // )
      },
    ),
  ),
)

#let resistor-mark = (
  inherit: "square",
  size: 4,
  fill: none,
  // tip-end: mark => -0.5 / calc.sin(mark.sharpness),
  tip-end: mark => -0.6 * mark.size / (20deg / mark.sharpness),
  tail-end: mark => 0.6 * mark.size / (20deg / mark.sharpness),
  tip-origin: mark => 0.6 * mark.size / (20deg / mark.sharpness),
  tail-origin: mark => -0.6 * mark.size / (20deg / mark.sharpness),
  // tip-end: mark => 1 * mark.size,
  // tail-end: mark => -1 * mark.size,
  // tail-origin: mark => -mark.size,
  // tip-origin: mark => mark.size,
  sharpness: 29deg,
  stroke: (
    // paint: brown.lighten(35%),
    thickness: 5pt,
  ),
  pos: 0.5,
  draw: mark => {
    // fletcher.draw.polygon((0,0), 3)
    // fletcher.draw.line(
    //   (180deg + mark.sharpness, mark.size),
    //   (0, 0),
    //   (180deg - mark.sharpness, mark.size),
    //   (180deg + mark.sharpness, mark.size),
    // )
    // fletcher.draw.line((92deg, mark.size / 1.9), (-92deg, mark.size / 1.9))
    fletcher.draw.rect(
      // (-0.6 * mark.size / (20deg / mark.sharpness), 0.5 * mark.size),
      // (0.6 * mark.size / (20deg / mark.sharpness), -0.5 * mark.size),
      (-0.6 * mark.size / (20deg / mark.sharpness) + 0.3, 15pt),
      (0.6 * mark.size / (20deg / mark.sharpness) - 0.3, -15pt),
      stroke: 0pt,
      radius: 0pt,
      fill: white,
    )
    // fletcher.draw.line(
    //   (-0.6 * mark.size / (20deg / mark.sharpness), 0.0 * mark.size),
    //   (0.6 * mark.size / (20deg / mark.sharpness), 0.0 * mark.size),
    // )
    fletcher.draw.line(
      (-0.6 * mark.size / (20deg / mark.sharpness), 0.0 * mark.size),
      (-0.5 * mark.size / (20deg / mark.sharpness), 0.5 * mark.size),
      (-0.3 * mark.size / (20deg / mark.sharpness), -0.5 * mark.size),
      (-0.1 * mark.size / (20deg / mark.sharpness), 0.5 * mark.size),
      (0.1 * mark.size / (20deg / mark.sharpness), -0.5 * mark.size),
      (0.3 * mark.size / (20deg / mark.sharpness), 0.5 * mark.size),
      (0.5 * mark.size / (20deg / mark.sharpness), -0.5 * mark.size),
      (0.6 * mark.size / (20deg / mark.sharpness), 0.0 * mark.size),
    )
  },
)

#let resistor(value, ..args) = edge(
  ..args,
  stroke: (
    paint: brown.lighten(25%),
    thickness: 10pt,
  ),
  layer: 1,
  marks: (
    resistor-mark,
  ),
  label: rect(
    text([*$#value Omega$*], size: 13pt),
    fill: white,
    radius: 6pt,
    stroke: brown.lighten(25%),
    inset: 3pt,
  ),
  // label-angle: -90deg,
  // label-side: left,
  label-sep: 0pt,
  label-angle: auto,
  label-anchor: "center",
)

// #(
//   range(1, 7)
//     .map(e => ((e,) * 7).enumerate())
//     .flatten()
//     .chunks(2)
//     .filter(e => e.at(0) != 5)
//     .map(e => (
//       str(e.at(1))
//         + str(numbering("a", e.at(0) + (if (e.at(0) > 5) { 0 } else { 1 })))
//     ))
//     .chunks(6)
// )

// #((-3, -2).map(rail_col => range(1, 31)
//     .map(e => ((e,) * 2).enumerate())
//     .flatten()
//     .chunks(2)
//     // .filter(e => e.at(0) != 5) // .map(e => breadboard_hole(pos: (2,2))),
//     .map(e =>
//       "prail_"
//         + str(r)
//         + str((r - 0.45) / 5)
//         + if (c < 0) { "l" } else { "r" }
//         + if (calc.rem-euclid(c, 2) == 0) { "p" } else { "n" },
//     )))
// #(
//   (-5, -4, 14, 15).map(c => range(1, 31)
//     .filter(e => calc.rem-euclid(e, 6) != 0)
//     .map(r => (
//       "prail_"
//         + if (c < 0) { "left" } else { "right" }
//         + "_"
//         + if (calc.rem-euclid(c, 2) == 0) { "positive" } else { "negative" }
//         + "_"
//         + "b"
//         + str(int(r / 6))
//         + ","
//         + str(calc.rem-euclid(r, 6))
//     )))
// )

#diagram(
  // Pin holes.
  ..range(1, 31)
    .map(e => ((e,) * 11).enumerate())
    .flatten()
    .chunks(2)
    .filter(e => e.at(0) != 5) // .map(e => breadboard_hole(pos: (2,2))),
    .map(e => breadboard_hole(pos: e, name: label(
      str(e.at(1))
        + str(numbering("a", e.at(0) + (if (e.at(0) > 5) { 0 } else { 1 }))),
    ))),

  // Row numbers.
  ..(-1, 11).map(c => range(1, 31).map(r => node(
    (c, r),
    shape: rect,
    stroke: black,
    align(center + horizon, text(size: 30pt, str(r))),
  ))),

  // Column letters.
  ..(0, 31).map(r => range(0, 11)
    .filter(e => e != 5)
    .map(c => node((c, r), shape: rect, align(center + horizon, text(
      size: 30pt,
      numbering("a", c + (if (c > 5) { 0 } else { 1 })),
    ))))),

  // Power rails.
  ..(-6, -5, 15, 16).map(c => range(1, 31)
    .filter(e => calc.rem-euclid(e, 6) != 0)
    .map(r => breadboard_hole(pos: (c, r + 0.45), name: label(
      "prail_"
        + if (c < 0) { "left" } else { "right" }
        + "_"
        + if ((int(c < 0)).bit-xor(int(calc.rem-euclid(c, 2) == 0)) == 0) {
          "positive"
        } else { "negative" }
        + "_"
        + "b"
        + str(int(r / 6))
        + "-"
        + str(calc.rem-euclid(r, 6)),
    )))),
  edge((14, 0.68), (14, 30.32), stroke: (
    paint: red.darken(10%),
    thickness: 5pt,
    cap: "square",
  )),
  edge((17, 0.68), (17, 30.32), stroke: (
    paint: blue.darken(10%),
    thickness: 5pt,
    cap: "square",
  )),
  edge((-7, 0.68), (-7, 30.32), stroke: (
    paint: red.darken(10%),
    thickness: 5pt,
    cap: "square",
  )),
  edge((-4, 0.68), (-4, 30.32), stroke: (
    paint: blue.darken(10%),
    thickness: 5pt,
    cap: "square",
  )),
  ..(
    ((-7, 0), "+"),
    ((-4, 0), "-"),
    ((14, 0), "+"),
    ((17, 0), "-"),
    ((-7, 31), "+"),
    ((-4, 31), "-"),
    ((14, 31), "+"),
    ((17, 31), "-"),
  ).map(((pos, sign)) => node(
    pos,
    place(
      horizon + center,
      dy: if (pos.at(1) <= 15) { -5pt } else { -25pt },
      dx: 5.25pt,
      text(sign, weight: 700, size: 100pt, fill: (
        if (sign == "+") { red } else { blue }
      ).darken(10%)),
    ),
    width: 1pt,
    height: 1pt,
  )),

  // node((-6, 0), place(horizon + center, dy: -5pt, dx: 5.25pt, text("+", weight: 700, size: 100pt, fill: red.darken(10%))), width: 1pt, height: 1pt),
  // node((-3, 0), place(horizon + center, dy: -5pt, dx: 5.25pt, text("-", weight: 700, size: 100pt, fill: blue.darken(10%))), width: 1pt, height: 1pt),
  // node((13, 0), place(horizon + center, dy: -5pt, dx: 5.25pt, text("+", weight: 700, size: 100pt, fill: red.darken(10%))), width: 1pt, height: 1pt),
  // node((16, 0), place(horizon + center, dy: -5pt, dx: 5.25pt, text("-", weight: 700, size: 100pt, fill: blue.darken(10%))), width: 1pt, height: 1pt),
  // node((-6,0), width: 5pt, height: 5pt, shape: rect, stroke: 1pt, place(center + horizon, [
  //   #align(center + horizon, [#rect(width: 40pt)])
  // ])),

  node(stroke: black, enclose: ((-2, -1), (12, 32))),
  node(stroke: black, enclose: ((-8, -1), (-3, 32))),
  node(stroke: black, enclose: ((13, -1), (18, 32))),


  // **====================================**
  // ||          <<<<< PCBs >>>>>          ||
  // **====================================**

  // ----- RPI PICO -----
  node(
    stroke: green,
    fill: green.lighten(10%).transparentize(70%),
    enclose: (pico_top_left, pico_bottom_right),
    name: <rpi_pico>,
    inset: 15pt,
    layer: 2,
  ),
  ..range(1, 21).map(r => node(
    // (rel: (0, 2), to: label(str(r) + "c")),
    (rel: (0, r - 1), to: pico_top_left),
    // (r + 20, 5),
    // (rel: (30, 10), to: <1e>),
    text(
      if (calc.rem-euclid(r, 5) == 3) { "GND" } else {
        "GP" + str(r - 1 - calc.floor((r + 1) / 5))
      },
      size: 22pt,
      fill: white,
      weight: 900,
    ),
    layer: 3,
    shape: circle,
    // fill: luma(50%).transparentize(20%),
    fill: (if (calc.rem-euclid(r, 5) == 3) { black } else { luma(50%) }),
    radius: 5pt,
    extrude: 25pt,

    // name: label(
    // "prail_"
    //   + if (c < 0) { "left" } else { "right" }
    //   + "_"
    //   + if (calc.rem-euclid(c, 2) == 0) { "positive" } else { "negative" }
    //   + "_"
    //   + "b"
    //   + str(int(r / 6))
    //   + ","
    //   + str(calc.rem-euclid(r, 6)),
  )),
  ..range(0, 9).map(r => node(
    (rel: (0, -r), to: pico_bottom_right),
    text(
      if (calc.rem-euclid(r, 5) == 2) { "GND" } else {
        "GP" + str(16 + r - calc.floor((r + 3) / 5))
      },
      size: 22pt,
      fill: white,
      weight: 900,
    ),
    layer: 3,
    shape: circle,
    // fill: luma(50%).transparentize(20%),
    fill: (if (calc.rem-euclid(r, 5) == 2) { black } else { luma(50%) }),
    radius: 5pt,
    extrude: 25pt,
  )),
  ..(
    (40, "5V", red),
    (38, "GND", black),
    (36, "3V", orange),
    (30, "RUN", brown.mix(red.lighten(50%))),
  ).map(((pin, name, clr)) => node(
    (rel: (0, 21 - pin), to: pico_bottom_right),
    text(
      // if (calc.rem-euclid(r, 5) == 2) {"GND"} else {"GP" + str(16 + r - calc.floor((r + 3 ) / 5))},
      name,
      size: 22pt,
      fill: white,
      weight: 900,
    ),
    layer: 3,
    shape: circle,
    // fill: luma(50%).transparentize(20%),
    // fill: (if (calc.rem-euclid(r, 5) == 2) {black} else {luma(50%)}),
    fill: clr,
    radius: 5pt,
    extrude: 25pt,
  )),


  // ----- TMC2209 -----
  node(
    stroke: teal,
    fill: teal.lighten(10%).transparentize(70%),
    enclose: (<23d>, <30g>),
    name: <rpi_pico>,
    inset: 15pt,
    layer: 2,
  ),
  ..(
    (1, "EN", luma(50%)),
    (2, "MS1", luma(50%)),
    (3, "MS2", luma(50%)),
    (4, "RX", luma(50%)),
    (5, "TX", luma(50%)),
    (6, "CLK", luma(50%)),
    (7, "STEP", luma(50%)),
    (8, "DIR", luma(50%)),
    (9, "VM", luma(50%)),
    (10, "GNDM", luma(50%)),
    (11, "A2", luma(50%)),
    (12, "A1", luma(50%)),
    (13, "B1", luma(50%)),
    (14, "B2", luma(50%)),
    (15, "VDD", luma(50%)),
    (16, "GNDC", luma(50%)),
  ).map(((pin, name, clr)) => node(
    (
      rel: (0, calc.rem-euclid(8 - pin, 8)),
      to: if (pin >= 9) { <23d> } else { <23g> },
    ),
    text(
      // if (calc.rem-euclid(r, 5) == 2) {"GND"} else {"GP" + str(16 + r - calc.floor((r + 3 ) / 5))},
      name,
      size: 22pt,
      fill: white,
      weight: 900,
    ),
    layer: 3,
    shape: circle,
    // fill: luma(50%).transparentize(20%),
    // fill: (if (calc.rem-euclid(r, 5) == 2) {black} else {luma(50%)}),
    fill: clr,
    radius: 5pt,
    extrude: 25pt,
  )),


  // **======================================**
  // ||          <<<<< WIRING >>>>>          ||
  // **======================================**

  wire(orange, <20e>, <23e>),
  wire(blue, <17j>, <23j>),
  wire(green, <19f>, <24f>, shift: 6pt),
  wire(brown, <18f>, <28f>, shift: -6pt),
  wire(purple, <22i>, <29i>, shift: -6pt),
  wire(white, <21i>, <30i>, shift: 6pt),
  wire(red, <24e>, ((), "-|", (<24e>, 30%, <7g>)), ((), "|-", <7g>), <7g>),
  wire(black, <5b>, <prail_left_negative_b0-5>),

  // Reset button.
  wire(brown, <13j>, (20, 13), bend: 18deg),
  wire(black, <15j>, (20, 13), bend: 8deg),
  node(
    (20, 13),
    text(size: 30pt, [To Reset Button]),
    layer: 3,
    shape: rect,
    stroke: 2pt,
    fill: white,
  ),

  // LS1
  wire(white.darken(10%), <18b>, (-10, 16)),
  wire(black, (-10, 16), <prail_left_negative_b2-4>),
  node(
    (-10, 16),
    text(size: 30pt, [To Limit Switch 1]),
    layer: 3,
    shape: rect,
    stroke: 2pt,
    fill: white,
  ),

  // LS2
  wire(blue, <19b>, (-10, 17)),
  wire(black, (-10, 17), <prail_left_negative_b3-1>, bend: -15deg),
  node(
    (-10, 17),
    text(size: 30pt, [To Limit Switch 2]),
    layer: 4,
    shape: rect,
    stroke: 2pt,
    fill: white,
  ),

  // Motor Power Supply
  wire(red, <30a>, (-10, 27)),
  wire(black, <29a>, (-10, 26)),
  node(
    (-10, 27),
    text(size: 30pt, [To Motor Power Supply Positive]),
    width: 220pt,
    shape: rect,
    stroke: 2pt,
    layer: 3,
    fill: white,
  ),
  node(
    (-10, 26),
    text(size: 30pt, [To Motor Power Supply Negative]),
    width: 220pt,
    shape: rect,
    stroke: 2pt,
    layer: 3,
    fill: white,
  ),


  capacitor(<29b>, <30b>),

  led(red, <3a>, <1a>, bend: -50deg),
  resistor(150, <prail_left_negative_b0-1>, <1b>, bend: 45deg),
  led(yellow, <4b>, <2b>),
  resistor(150, <prail_left_negative_b0-2>, <2a>),

  wire(blue, <6e>, <2f>),
  led(blue, <2i>, <1i>),
  resistor(100, <1j>, <5j>),

  // led(yellow, <4c>, <2c>),
  // resistor(150, <prail_left_negative_b0-2>, <2a>),
)

// #context {
//   // let mark = fletcher.MARKS.get().stealth
//   let mark = resistor-mark
//
//   mark += (angle: 90deg, sharpness: 50deg)
//   fletcher.mark-debug(mark)
// }
