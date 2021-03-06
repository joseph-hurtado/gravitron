open GravShared;
open SharedTypes;

open Reprocessing;

open Enemy;

let initialSpeed = isPhone ? 1. : 2.;
let sizeFactor = isPhone ? 0.8 : 1.0;

let w = env => Env.width(env) |> float_of_int;
let h = env => Env.height(env) |> float_of_int;

let pos1 = (env) => {
  isPhone
  ? (w(env) /. 2., w(env) /. 4.)
  : (w(env) /. 4., h(env) /. 4.)
};

let pos2 = (env) => {
  isPhone
  ? (
    (w(env) /. 2., w(env) /. 4.),
    (w(env) /. 2., h(env) -. w(env) /. 4.)
  )
  : (
    (w(env) /. 4., h(env) /. 4.),
    (w(env) *. 3. /. 4., h(env) -. w(env) /. 4.)
  )
};

let pos3 = (env) => {
  isPhone
  ? (
    (w(env) /. 4., w(env) /. 4.),
    (w(env) *. 3. /. 4., w(env) /. 4.),
    (w(env) *. 3. /. 4., h(env) -. w(env) /. 4.)
  )
  : (
    (w(env) /. 4., h(env) /. 4.),
    (w(env) *. 3. /. 4., w(env) /. 4.),
    (w(env) *. 3. /. 4., h(env) *. 3. /. 4.)
  )
};

let pos4 = (env) => {
  isPhone
  ? (
    (w(env) /. 4., w(env) /. 4.),
    (w(env) /. 4., h(env) -. w(env) /. 4.),
    (w(env) *. 3. /. 4., h(env) -. w(env) /. 4.),
    (w(env) *. 3. /. 4., w(env) /. 4.)
  )
  : (
    (w(env) /. 4., h(env) /. 4.),
    (w(env) /. 4., h(env) *. 3. /. 4.),
    (w(env) *. 3. /. 4., h(env) *. 3. /. 4.),
    (w(env) *. 3. /. 4., h(env) /. 4.)
  )
};

let place1 = (env, enemy) => [{...enemy, pos: pos1(env)}];

let place2 = (env, e1, e2) => {
  let (p1, p2) = pos2(env);
  [{...e1, pos: p1}, {...e2, pos: p2}]
};

let place3 = (env, e1, e2, e3) => {
  let (p1, p2, p3) = pos3(env);
  [{...e1, pos: p1}, {...e2, pos: p2}, {...e3, pos: p3}]
};

let place4 = (env, e1, e2, e3, e4) => {
  let (p1, p2, p3, p4) = pos4(env);
  [{...e1, pos: p1}, {...e2, pos: p2}, {...e3, pos: p3}, {...e4, pos: p4}]
};

open Enemy;

let fromHex = (h1, h2) => {
  let code = c => switch c {
  | '0'..'9' => Char.code(c) - 48
  | 'A'..'F' => Char.code(c) - 55
  | 'a'..'f' => Char.code(c) - 87
  | _ => invalid_arg("Hex.to_char: %d is an invalid char", Char.code(c))
  };
  let a = code(h1);
  let b = code(h2);
  a * 16 + b;
};


let hexColor = hash => {
  let r = fromHex(hash.[0], hash.[1]);
  let g = fromHex(hash.[2], hash.[3]);
  let b = fromHex(hash.[4], hash.[5]);
  Utils.color(~r, ~g, ~b, ~a=255)
};






/* THe actual levles n stuff */

let red = Enemy.basic(~start=200., ~speed=initialSpeed, ~full=300., hexColor("ff0000"));
let blue = Enemy.basic(~start=0.,  ~speed=initialSpeed, ~full=200., ~health=2, ~damage=3, Reprocessing.Constants.blue);

let stage1 = env => [|
  place1(env, red),
  place2(env, red, red |> startTimer(100.)),
  place2(env, red, blue |> startTimer(50.)),
  place2(env, blue, blue |> startTimer(150.)),
  place4(
    env,
    blue |> startTimer(0.),
    blue |> startTimer(100.),
    blue |> startTimer(150.),
    blue |> startTimer(50.)
  )
|];

let green = {
  ...Enemy.basic(~start=200., ~full=300., ~speed=initialSpeed, ~health=3, Reprocessing.Constants.green),
  shooting: TripleShot(Bullet.basic(3))
};

let yellow = {
  ...Enemy.basic(~start=250., ~full=300., ~speed=initialSpeed, ~health=5, hexColor("D1CF00")),
  shooting: OneShot(Bullet.basic(~moving=HeatSeeking(0.3, 5.), 10))
};

let wanderBlue = {...blue, movement: Wander((0., 0.))};

let revengeBlue = {
  ...wanderBlue,
  dying: Revenge(5, Bullet.basic(5))
};

let stage2 = env => [|
  place1(env, green),
  place3(env, red, blue, green),
  place2(env, green, revengeBlue),
  place3(env, revengeBlue, green, revengeBlue),
  place3(env, green, revengeBlue, yellow)
|];

let orange = {
  ...Enemy.basic(~start=200., ~full=300., ~health=3, hexColor("FF724A")),
  shooting: OneShot(Bullet.basic(~stepping=TimeBomb((0., 150.)), 5))
};

let wanderYellow = {...yellow, movement: Wander((0., 0.))};

let pink = {
  ...Enemy.basic(~start=200., ~full=300., ~health=4, hexColor("FF42DB")),
  shooting: OneShot(Bullet.basic(~stepping=Scatter((0., 80.), 5, Bullet.basic(3)), 10))
};

let fastPink = {
  ...Enemy.basic(~start=200., ~full=300., ~health=4, hexColor("FF42DB")),
  shooting: OneShot(Bullet.basic(~stepping=Scatter((0., 40.), 5, Bullet.basic(3)), 10))
};

let stage3 = env => [|
  place1(env, orange),
  place2(env, orange, wanderYellow),
  place3(env, green, wanderBlue, orange),
  place2(env, pink, orange |> startTimer(0.)),
  place4(env,
    wanderYellow |> startTimer(0.),
    pink |> startTimer(250.),
    orange |> startTimer(150.),
    fastPink
  ),
|];

let teal = {
  ...Enemy.basic(~start=100., ~full=150., ~health=4, hexColor("00FFAD")),
  shooting: Alternate(
    Bullet.basic(
      ~moving=Mine(30., 60., (0., 0.)),
      5
    ),
    Bullet.basic(5),
    false
  )
};

let baby = {
  ...Enemy.basic(~start=100., ~full=150., ~health=5, hexColor("00DDFF")),
  shooting: OneShot(Bullet.basic(
    ~moving=Mine(40., 70., (0., 0.)),
    ~stepping=ProximityScatter(110., 5, Bullet.basic(3)),
    15
  ))
};

let stage4 = env => [|
  place1(env, teal),
  place2(env, teal, teal |> startTimer(50.)),
  place2(env, baby, wanderBlue),
  place2(env, baby, baby),
  place3(env, baby, teal, wanderBlue),
|];

let purple = {
  ...Enemy.basic(~start=100., ~full=150., ~health=3, hexColor("8000FF")),
  dying: Asteroid
};

let lightPurple = {
  ...Enemy.basic(~start=100., ~full=150., ~health=3, hexColor("B66BFF")),
  shooting: OneShot(Bullet.basic(~stepping=TimeBomb((0., 150.)), 5)),
  dying: Asteroid
};

let oliveGreen = {
  ...Enemy.basic(~start=100., ~full=150., ~health=5, hexColor("5BA802")),
  shooting: OneShot(Bullet.basic(
    ~moving=Mine(40., 70., (0., 0.)),
    ~stepping=Shooter((0., 200.), Bullet.basic(3)),
    15
  ))
};

let stage5 = env => [|
  place1(env, purple),
  place2(env, purple, green),
  place2(env, green, lightPurple),
  place1(env, oliveGreen),
  place3(env, oliveGreen, lightPurple, wanderBlue),
|];

let white = {
  ...Enemy.basic(~start=200., ~speed=initialSpeed, ~full=300., hexColor("ffffff")),
  movement: GoToPosition((0., 0.)),
  stepping: Rabbit(600., (450., 600.))
};

let rabbitTime = (time, enemy) => switch enemy.stepping {
| Rabbit(min, (_, max)) => {...enemy, stepping: Rabbit(min, (time, max))}
| _ => assert(false)
};

let gray = {
  ...white,
  color: hexColor("dddddd"),
  health: (3, 3)
};

let darkGray = {
  ...white |> rabbitTime(200.),
  color: hexColor("999999"),
  health: (4, 4),
  shooting: OneShot(Bullet.basic(~stepping=ProximityScatter(100., 5, Bullet.basic(3)), 15))
};

let stage6 = env => [|
  place1(env, white),
  place3(env, white |> rabbitTime(350.), white |> rabbitTime(250.), white),
  place2(env, gray, wanderBlue),
  place1(env, darkGray),
  place2(env, darkGray, oliveGreen),
|];

let stages = env => [|
  stage1(env),
  stage2(env),
  stage3(env),
  stage4(env),
  stage5(env),
  stage6(env),
|] |> Array.map(Array.map(List.map(Enemy.fixMoving)));

