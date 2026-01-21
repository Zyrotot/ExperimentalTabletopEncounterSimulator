package entity

import (
	"testing"

	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

func expectDR(t *testing.T, c *Character, idx int, value int, bypass rules.DamageType) {
	t.Helper()

	if len(c.Runtime.AddedDR) <= idx {
		t.Fatalf("expected DR index %d to exist, DR list: %+v", idx, c.Runtime.AddedDR)
	}

	dr := c.Runtime.AddedDR[idx]

	if dr.Value != value {
		t.Fatalf("expected DR value %d, got %d", value, dr.Value)
	}

	if dr.BypassType != bypass {
		t.Fatalf("expected bypass type %q, got %q", bypass, dr.BypassType)
	}
}

func TestAddDR_NewBypassType(t *testing.T) {
	c := &Character{}

	c.AddDR(DamageReduction{
		Value:      5,
		BypassType: rules.Good,
	})

	expectDR(t, c, 0, 5, rules.Good)
}

func TestAddDR_IncreaseDR(t *testing.T) {
	c := &Character{}

	dr := DamageReduction{
		Value:      1,
		BypassType: "",
	}

	c.AddDR(dr)
	expectDR(t, c, 0, 1, "")

	c.AddDR(dr)
	expectDR(t, c, 0, 2, "")
}

func TestAddDR_IncreaseDR_AndNew(t *testing.T) {
	c := &Character{}

	dr_all := DamageReduction{
		Value:      1,
		BypassType: "",
	}

	dr_good := DamageReduction{
		Value:      1,
		BypassType: rules.Good,
	}

	c.AddDR(dr_all)
	expectDR(t, c, 0, 1, "")

	c.AddDR(dr_good)
	expectDR(t, c, 1, 1, "good")

	c.AddDR(dr_all)
	expectDR(t, c, 0, 2, "")
}

func TestAddDR_IncreaseDR_RemoveDR(t *testing.T) {
	c := &Character{}

	dr_all := DamageReduction{
		Value:      1,
		BypassType: "",
		Source:     "test",
	}

	c.AddDR(dr_all)
	if len(c.Runtime.AddedDR) != 1 {
		t.Fatalf("Expected 1 DR instance")
	}

	c.RemoveDRBySource("test")
	if len(c.Runtime.AddedDR) != 0 {
		t.Fatalf("Expected no DR instance")
	}
}
