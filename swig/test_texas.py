#!/usr/bin/env python
import wx
import libqp

USE_BUFFERED_DC = True

class GameFrame(wx.Frame):
    """ Draw a line to a pannel."""
    
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(820,580))
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
        
        # control
        self.btn_start = wx.Button(self, id=-1, label='NewGame',
            pos=(0,0), size=(100,28))
        self.btn_start.Bind(wx.EVT_BUTTON, self.ButtonStartClick)
        self.btn_start.SetToolTip(wx.ToolTip("start a new game"))

        self.btn_3card = wx.Button(self, id=-1, label='Draw three card',
            pos=(0,0), size=(100,28))
        self.btn_3card.Bind(wx.EVT_BUTTON, self.Button3CardClick)
        self.btn_3card.SetToolTip(wx.ToolTip("draw three card"))
        
        self.btn_zcard = wx.Button(self, id=-1, label='Draw zcard',
            pos=(0,0), size=(100,28))
        self.btn_zcard.Bind(wx.EVT_BUTTON, self.ButtonZCardClick)
        self.btn_zcard.SetToolTip(wx.ToolTip("draw zcard"))

        self.btn_hcard = wx.Button(self, id=-1, label='Draw hcard',
            pos=(0,0), size=(100,28))
        self.btn_hcard.Bind(wx.EVT_BUTTON, self.ButtonHCardClick)
        self.btn_hcard.SetToolTip(wx.ToolTip("draw hcard"))

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
        
        self.infoW = 58
        self.blankH = 30
        self.upH = 19
        self.pokerOffsetW = 21
        self.rcHand = []
        self.rcHand.append(wx.Rect(self.infoW, 0, 0, 0))
        self.rcHand.append(wx.Rect(self.infoW, 0, 0, 0))
        self.flag_up = [0] * 50
        
        # create game logic
        self.texas = libqp.texas_new()
        self.texas.player_num = 2
        self.NewGame()

    def NewGame(self):
        libqp.texas_start(self.texas)

        # sort
        for i in range(0, 3):
            player = libqp.texas_get_player(self.texas, i)
            hand = libqp.texas_get_player_hand(player)
            libqp.texas_sort(hand)
        # recalc all rect
        self.CalcRect()
        self.Refresh()        
    
    def ButtonStartClick(self, e):
        self.NewGame()

    def Button3CardClick(self, e):
        libqp.texas_set_state(self.texas, 2)
        self.Refresh()

    def ButtonZCardClick(self, e):
        libqp.texas_set_state(self.texas, 3)
        self.Refresh()

    def ButtonHCardClick(self, e):
        libqp.texas_set_state(self.texas, 4)
        self.Refresh()
                
    def OnPaint(self, e):
        width, height = self.GetClientSize()
        
        if USE_BUFFERED_DC:
            dc = wx.BufferedPaintDC(self, self._Buffer)
        else:
            dc = wx.PaintDC(self)
        brBack = wx.Brush('#053154', wx.SOLID);
        dc.SetBackground(brBack)
        dc.Clear()
        
        pokerW, pokerH = self.bmpPoker['0'].GetSize()
        x = 0
        y = 0
        
        max_no = 0
        max_handtype = libqp.hand_type()

        # draw player's hand cards
        dc.SetTextForeground(wx.WHITE)
        for i in range(0, 2):
            x = 0
            cp_no = self.texas.curr_player_no
            playerid = "player%d" % (1 - i)
            dc.DrawText(playerid, x, self.rcHand[1-i].GetY())
            if cp_no == (1 - i):
                # draw arrow
                arlist = [(0,0), (8,8), (0,16)]
                dc.DrawPolygon(arlist, self.rcHand[1-i].GetX() - 10,
                    self.rcHand[1-i].GetY() + pokerH / 2 - 8)

            player = libqp.texas_get_player(self.texas, 1 - i)
            hand = libqp.texas_get_player_hand(player)
            num = libqp.hand_num(hand)
            x = self.rcHand[1-i].GetX()
            y = self.rcHand[1-i].GetY()
            for j in range(0, num):
                card = libqp.hand_get(hand, j)
                keyname = libqp.card_text(card)
                if self.flag_up[j] > 0 and cp_no == (1 - i):
                    dc.DrawBitmap(self.bmpPoker[keyname], x, y - self.upH, 0)
                else:
                    dc.DrawBitmap(self.bmpPoker[keyname], x, y, 0)
                x = x + pokerW + 2
            
            if libqp.texas_get_state(self.texas) >= 2:
                rx = x + 10;
                htresult = libqp.hand_new(5)
                hand_all = libqp.hand_new(7)
                libqp.texas_group(self.texas, 1-i, hand_all)
                #libqp.hand_dump(hand_all, 7)
                htype = libqp.hand_type()
                libqp.hand_zero(htresult)
                r = libqp.texas_handtype(hand_all, htype, htresult)
                #print r,htype.logic_value1
                #libqp.hand_dump(htresult, 5)
                if i == 0:
                    max_handtype.type = htype.type
                    max_handtype.logic_value1 = htype.logic_value1
                    max_handtype.logic_value2 = htype.logic_value2
                    max_handtype.logic_value3 = htype.logic_value3
                    max_no = 1-i
                else:
                    #print "ht1=%d,ht2=%d" % (max_handtype.type,htype.type)
                    mflag = libqp.texas_compare(max_handtype, htype)
                    if mflag < 0:
                        max_handtype.type = htype.type
                        max_handtype.logic_value1 = htype.logic_value1
                        max_handtype.logic_value2 = htype.logic_value2
                        max_handtype.logic_value3 = htype.logic_value3
                        max_no = 1-i

                for j in range(0, libqp.hand_num(htresult)):
                    card = libqp.hand_get(htresult, j)
                    keyname = libqp.card_text(card)
                    dc.DrawBitmap(self.bmpPoker[keyname], rx, y, 0)
                    rx = rx + self.pokerOffsetW
                ht_x = rx + 10 + pokerW;
                ht_y = y
                if htype.type == libqp.TEXAS_HIGHCARD:
                    htstr = "TEXAS_HIGHCARD"
                elif htype.type == libqp.TEXAS_PAIR1:
                    htstr = "TEXAS_PAIR1"
                elif htype.type == libqp.TEXAS_PAIR2:
                    htstr = "TEXAS_PAIR2"
                elif htype.type == libqp.TEXAS_THREE:
                    htstr = "TEXAS_THREE"
                elif htype.type == libqp.TEXAS_STRAIGHT:
                    htstr = "TEXAS_STRAIGHT"
                elif htype.type == libqp.TEXAS_FLUSH:
                    htstr = "TEXAS_FLUSH"
                elif htype.type == libqp.TEXAS_FULLHOUSE:
                    htstr = "TEXAS_FULLHOUSE"
                elif htype.type == libqp.TEXAS_FOUR:
                    htstr = "TEXAS_FOUR"
                elif htype.type == libqp.TEXAS_STRAIGHT_FLUSH:
                    htstr = "TEXAS_STRAIGHT_FLUSH"
                elif htype.type == libqp.TEXAS_ROYAL:
                    htstr = "TEXAS_ROYAL"
                else:
                    htstr = "%d" % htype.type
                dc.DrawText(htstr, ht_x, ht_y)
                libqp.hand_free(htresult)
                libqp.hand_free(hand_all)
               
        # draw five common card
        num = 0
        if self.texas.game_state == 2:
            num = 3
        if self.texas.game_state == 3:
            num = 4
        if self.texas.game_state == 4:
            num = 5
        x = (width - (num * pokerW + (num - 1) * 2)) / 2
        for i in range(0, num):
            card = libqp.texas_get_board(self.texas, i)
            keyname = libqp.card_text(card)
            dc.DrawBitmap(self.bmpPoker[keyname], x, 0, 0)
            x = x + pokerW + 2
        
        # info
        info = "state:%d\n" % (self.texas.game_state)
        if num >= 3:
            info += "max player:%d\n" % max_no

        #info += "curr player:%d\n" % (self.texas.curr_player_no)
        dc.DrawText(info, 0, 0)
        #dc.SetPen(wx.Pen('#007F0F', 4))
        #dc.DrawLine(0, 0, 50, 50)
    
    def OnEraseBackground(self, e):
        # Do nothing, to avoid flashing on MSW
        pass

    def OnLeftDown(self, e):
        pokerW, pokerH = self.bmpPoker['0'].GetSize()
        pt = e.GetPosition()

    # reponse mouse right button
    def OnRightDown(self, e):
        pt = e.GetPosition()
    
    def CalcRect(self):
        width, height = self.GetClientSize()
        pokerW, pokerH = self.bmpPoker['0'].GetSize()
        y = height
        for i in range(0, 2):
            self.rcHand[1-i].SetY(y - pokerH)
            player = libqp.texas_get_player(self.texas, 1 - i)
            hand = libqp.texas_get_player_hand(player)
            num = libqp.hand_num(hand)
            self.rcHand[1-i].SetWidth(pokerW + (num - 1) * self.pokerOffsetW)
            self.rcHand[1-i].SetHeight(pokerH)
            y = y - pokerH - self.blankH

    # reponse window resize
    def OnSize(self, e):
        width, height = self.GetClientSize()
        self._Buffer = wx.EmptyBitmap(width, height)
        
        self.btn_start.MoveXY(width - 110, height - 50)
        self.btn_3card.MoveXY(width - 110, height - 86)
        self.btn_zcard.MoveXY(width - 110, height - 122)
        self.btn_hcard.MoveXY(width - 110, height - 158)
        self.CalcRect()
        self.Refresh()
        
app = wx.App(False)
frame = GameFrame(None, 'libqp test app - texas')
frame.Show()
app.MainLoop()
