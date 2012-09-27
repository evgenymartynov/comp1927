polygon :: [(Num, Num)] -> [Shape]
polygon list =
    let
        halp :: (Num, Num) -> (Num, Num) -> [(Num, Num)] -> [Shape]
        halp first p list =
            match list with
                [] -> [Line p first]
            |   y:ys -> (Line p y):(halp first y ys)
            end;

        blah :: (Num, Num) -> [(Num, Num)] -> [Shape]
        blah first list =
            match list with
                y:ys -> (Line first y):(halp first y ys)
            end;
    in
    match list with
        x:xs -> blah x xs
    end;

rectangle :: Num -> Num -> Num -> Num -> [Shape]
rectangle x1 y1 x2 y2 = polygon [(x1, y1), (x1, y2), (x2, y2), (x2, y1)];

scaleHorz :: Num -> Num -> Num -> Picture -> Picture
scaleHorz a b c pic =
    Beside (a+b) c (Beside a b Empty pic) Empty;

scaleVert :: Num -> Num -> Num -> Picture -> Picture
scaleVert a b c pic =
    Above (a+b) c (Above a b Empty pic) Empty;

roof :: Picture
roof = Canvas 10 10 (polygon [(0, 0), (10, 0), (5, 10)]);

wall :: Picture
wall = Canvas 10 10 (rectangle 1 0 9 10);

door :: Picture
door = Above 40 30 Empty (scaleHorz 20 10 20 wall);

window :: Picture
window = scaleHorz 10 5 5 (scaleVert 60 7 30 (Canvas 10 10 (rectangle 0 0 10 10)));

windows :: Picture
windows = Beside 50 50 window (Flip window);

house :: Picture
house = Overlay (Overlay (Above 30 70 roof wall) door) windows;

main = house