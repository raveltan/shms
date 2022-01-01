package main

import (
	"fmt"
	"log"
	"time"

	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/logger"
	"gorm.io/driver/sqlite"
	"gorm.io/gorm"
)

type Dht struct {
	ID        uint      `gorm:"primaryKey" json:"id,ommitempty"`
	CreatedAt time.Time `json:"date,ommitempty"`
	Temp      int8      `json:"t"`
	Humid     int8      `json:"h"`
}

type Water struct {
	ID        uint      `gorm:"primaryKey" json:"id,ommitempty"`
	CreatedAt time.Time `json:"date,ommitempty"`
	Amount    int32     `json:"a"`
}

func main() {
	db, err := gorm.Open(sqlite.Open("data.db"))
	if err != nil {
		panic(err)
	}

	db.AutoMigrate(&Dht{})
	db.AutoMigrate(&Water{})

	app := fiber.New()
	app.Use(logger.New())
	s := server{Db: db}
	app.Post("/dht", s.dhtPost)
	app.Post("/water", s.waterPost)
	app.Get("/dht", s.getDht)
	app.Get("/water", s.getWater)

	app.Listen(":8080")
}

type server struct {
	Db *gorm.DB
}

func (s *server) dhtPost(c *fiber.Ctx) error {
	var data Dht
	err := c.BodyParser(&data)
	if err != nil {
		log.Println(err)
		return c.SendStatus(fiber.StatusBadRequest)
	}

	s.Db.Create(&data)

	return c.SendStatus(fiber.StatusOK)
}

func (s *server) waterPost(c *fiber.Ctx) error {
	var data Water
	err := c.BodyParser(&data)
	if err != nil {
		log.Println(err)
		return c.SendStatus(fiber.StatusBadRequest)
	}

	s.Db.Create(&data)

	return c.SendStatus(fiber.StatusOK)
}

func (s *server) getDht(c *fiber.Ctx) error {
	var datas []Dht
	year := c.Query("year")
	month := c.Query("month")
	day := c.Query("day")
	date := fmt.Sprintf("%v-%v-%v", year, month, day)
	s.Db.Where("date(created_at) = date(?)", date).Find(&datas)
	return c.JSON(datas)
}
func (s *server) getWater(c *fiber.Ctx) error {
	var datas []Water
	year := c.Query("year")
	month := c.Query("month")
	day := c.Query("day")
	date := fmt.Sprintf("%v-%v-%v", year, month, day)
	s.Db.Where("data(created_at) = date(?)", date).Find(&datas)
	return c.JSON(datas)
}
