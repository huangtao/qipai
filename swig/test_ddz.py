#!/usr/bin/env python
import wx
import libqp

class GameFrame(wx.Frame):
    """ Draw a line to a pannel."""
    
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(800,500))
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        
        # load all cards image
        suit = ['S','H','C','D']
        rank = ['A','2','3','4','5','6','7','8','9','T','J','Q','K']
        self.bmpPoker = {}
        bmp = wx.EmptyBitmap(1, 1)
        bmp.LoadFile("../data/poker/0.png", wx.BITMAP_TYPE_ANY)
        self.bmpPoker['0'] = bmp
        for s in suit:
            for r in rank:
                keyname = "%s%s" % (s, r)
                bmpname = "../data/poker/%s.png" % keyname
                bmp = wx.EmptyBitmap(1, 1)
                bmp.LoadFile(bmpname, wx.BITMAP_TYPE_ANY)
                self.bmpPoker[keyname] = bmp
        bmp = wx.EmptyBitmap(1, 1)
        bmp.LoadFile("../data/poker/JS.png", wx.BITMAP_TYPE_ANY)
        self.bmpPoker['*S'] = bmp
        bmp = wx.EmptyBitmap(1, 1)
        bmp.LoadFile("../data/poker/JB.png", wx.BITMAP_TYPE_ANY)
        self.bmpPoker['*B'] = bmp
        
        # create game logic        
        self.ddz = libqp.ddz_new()
        libqp.ddz_start(self.ddz)
        # sort
        for i in range(0, 3):
            player = libqp.ddz_get_player(self.ddz, i)
            hand = libqp.ddz_get_player_hand(player)
            libqp.cards_sort(hand)
        
    def OnPaint(self, event=None):
        width, height = self.GetClientSize()
        
        dc = wx.PaintDC(self)
        brBack = wx.Brush('#053154', wx.SOLID);
        dc.SetBackground(brBack)
        dc.Clear()
        
        pokerW, pokerH = self.bmpPoker['0'].GetSize()

        #draw 3 di
        x = 0
        y = 0
        for i in range(0, 3):
            card = libqp.ddz_get_di(self.ddz, i)
            keyname = libqp.card_text(card)
            dc.DrawBitmap(self.bmpPoker[keyname], x, y, 0)
            x = x + pokerW + 6
            
        # draw player's hand cards
        y = height
        for i in range(0, 3):
            player = libqp.ddz_get_player(self.ddz, 2 - i)
            hand = libqp.ddz_get_player_hand(player)
            num = libqp.hand_num(hand)
            x = 0
            y = y - pokerH
            for j in range(0, num):
                card = libqp.hand_get(hand, j)
                keyname = libqp.card_text(card)
                dc.DrawBitmap(self.bmpPoker[keyname], x, y, 0)
                x = x + 18
            y = y - 30
        
        #dc.SetPen(wx.Pen('#007F0F', 4))
        #dc.DrawLine(0, 0, 50, 50)
        
app = wx.App(False)
frame = GameFrame(None, 'libqp test client')
frame.Show()
app.MainLoop()
