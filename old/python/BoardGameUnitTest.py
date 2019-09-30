import unittest

from BoardGame import BoardGame

"""
Purpose: Runs the unit tests
*    Date: April 1th, 2019
*    Authors: Chris Ikongo
*    Algorithm: None
"""


class MyTestCase(unittest.TestCase):

    def setUp(self):
        self.p1 = BoardGame("(K,Q,J,N)(I,I,K,G)(N,C,P,E)(E,O,C,Q)")

    def test_something(self):
        self.assertEqual(True, True)

    def test_qrcodeParsed(self):
        # p1 = BoardGame(self.qr)
        self.assertTrue(self.p1.parsed)

    # Get the parking location
    def test_getFirstLocation(self):
        # p1 = BoardGame(self.qr)
        self.assertEqual(self.p1.coordList[0].X1, 1150)
        self.assertEqual(self.p1.coordList[0].Y1, 1840)
        self.assertEqual(self.p1.coordList[0].X2, 1035)
        self.assertEqual(self.p1.coordList[0].Y2, 1495)

    # Get all the 3 pillars
    def test_getNextPillars(self):
        self.assertIsNotNone(self.p1.getNextPillor())
        self.assertIsNotNone(self.p1.getNextPillor())
        self.assertIsNotNone(self.p1.getNextPillor())
        self.assertIsNone(self.p1.getNextPillor())


if __name__ == '__main__':
    unittest.main()
