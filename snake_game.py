
import random
import turtle

# Window setup
screen = turtle.Screen()
screen.title("Snake Game - Team 11")
screen.bgcolor("black")
screen.setup(width=600, height=600)
screen.tracer(0)

# Snake head
head = turtle.Turtle()
head.shape("square")
head.color("lime")
head.penup()
head.goto(0, 0)

# Food
food = turtle.Turtle()
food.shape("circle")
food.color("red")
food.penup()
food.goto(0, 100)

# Score display
score = 0
score_writer = turtle.Turtle()
score_writer.hideturtle()
score_writer.color("white")
score_writer.penup()
score_writer.goto(0, 260)


def show_score():
    score_writer.clear()
    score_writer.write(
        f"Score: {score}",
        align="center",
        font=("Arial", 16, "normal")
    )


# Direction validation: reject an immediate reverse
direction = "stop"


def set_direction(new_direction):
    global direction

    opposite = {
        "up": "down",
        "down": "up",
        "left": "right",
        "right": "left"
    }

    if new_direction not in opposite:
        return

    if direction != "stop" and opposite[direction] == new_direction:
        return

    direction = new_direction


def move_head():
    x = head.xcor()
    y = head.ycor()

    if direction == "up":
        head.sety(y + 20)
    elif direction == "down":
        head.sety(y - 20)
    elif direction == "left":
        head.setx(x - 20)
    elif direction == "right":
        head.setx(x + 20)


def game_loop():
    global score

    if direction != "stop":
        move_head()

        # Wall collision
        if (head.xcor() > 290 or head.xcor() < -290
                or head.ycor() > 290 or head.ycor() < -290):
            score_writer.goto(0, 0)
            score_writer.write(
                f"GAME OVER! Score: {score}",
                align="center",
                font=("Arial", 20, "bold")
            )
            return

        # Food collection
        if head.distance(food) < 20:
            score += 1
            show_score()

            food.goto(
                random.randrange(-14, 15) * 20,
                random.randrange(-14, 15) * 20
            )

    screen.update()
    screen.ontimer(game_loop, 120)


# Keyboard controls
screen.listen()
screen.onkeypress(lambda: set_direction("up"), "Up")
screen.onkeypress(lambda: set_direction("down"), "Down")
screen.onkeypress(lambda: set_direction("left"), "Left")
screen.onkeypress(lambda: set_direction("right"), "Right")

show_score()
game_loop()
screen.mainloop()
